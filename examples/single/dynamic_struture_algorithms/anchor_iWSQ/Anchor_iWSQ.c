/* Idempotent Work Stealing
 * M. M. Michael, M. T. Vechev, V. A. Saraswat
 * PPoPP'09
 */
#include <pthread.h>
#include <stdlib.h>

#ifndef CPROVER
#define __CPROVER_atomic_begin() 
#define __CPROVER_atomic_end()
#define nondet() rand()%2
#endif

#define EMPTY -1
#define ROUNDS 10
#define MAX_SIZE 10

typedef int task_t;

typedef struct {
int size;
task_t* array;
} array_with_size_t;

typedef struct {
  int head;
  int size;
  int tag;
} anchor_t;

task_t shared_tasks[ROUNDS];
size_t counter=0;
array_with_size_t shared_arrays[ROUNDS];
size_t counter_arrays=0;

task_t* new_task_array(int size) {
  if(counter+(size_t)size<ROUNDS)
  {
    const size_t cnt=counter;
    counter+=(size_t)size;
    return shared_tasks+cnt;
  }
  else
    return NULL;
}

array_with_size_t* new_array_with_size() {
  if(counter<ROUNDS)
    return shared_arrays+counter_arrays++;
  else
    return NULL;
}

/* shared structures */
anchor_t anchor;
array_with_size_t* tasks;

/* constructor */
void DeIwsq(int size) {
  anchor.head=0;
  anchor.size=0;
  anchor.tag=0;
  tasks=new_array_with_size();
  tasks->array=new_task_array(size);
}

/* CAS */
_Bool CAS(anchor_t* anc, 
  int h_old, int s_old, int g_old, 
  int h_new, int s_new, int g_new) 
{
  _Bool preCond;
  __CPROVER_atomic_begin();
  preCond=(anc->head==h_old && anc->size==s_old && anc->tag==g_old);
  if(preCond) {
    anc->head=h_new;
    anc->size=s_new;
    anc->tag=g_new;
  }
  __CPROVER_atomic_end();
  return preCond;
}

void expand() {
  int h, s, g;
  __CPROVER_atomic_begin();
  h=anchor.head;
  s=anchor.size;
  g=anchor.tag;
  __CPROVER_atomic_end();
  array_with_size_t* new_tasks=new_array_with_size();
  new_tasks->array=new_task_array(s*2);
  new_tasks->size=2*s;
  int i;
  for(i=0; i<s; i++)
    new_tasks->array[(h+i)%new_tasks->size]=tasks->array[(h+i)%tasks->size];
  tasks=new_tasks;
}

void put(task_t task) {
  int h, s, g;
L1:
  __CPROVER_atomic_begin();
  h=anchor.head;
  s=anchor.size;
  g=anchor.tag;
  __CPROVER_atomic_end();
  if(s==tasks->size) {
    expand();
    goto L1;
  }
  tasks->array[(h+s)%tasks->size]=task;
  __CPROVER_atomic_begin();
  anchor.head=h;
  anchor.size=s+1;
  anchor.tag=g+1;
  __CPROVER_atomic_end();
}

task_t take() {
  int h, s, g;
  __CPROVER_atomic_begin();
  h=anchor.head;
  s=anchor.size;
  g=anchor.tag;
  __CPROVER_atomic_end();
  if(s==0)
    return EMPTY;
  task_t task=tasks->array[(h+s-1)%tasks->size];
  __CPROVER_atomic_begin();
  anchor.head=h;
  anchor.size=s-1;
  anchor.tag=g;
  __CPROVER_atomic_end();
  return task;
}

task_t steal() {
  int h, s, g;
L2:
  __CPROVER_atomic_begin();
  h=anchor.head;
  s=anchor.size;
  g=anchor.tag;
  __CPROVER_atomic_end();
  if(s==0)
    return EMPTY;
  array_with_size_t* ptasks=tasks;
  task_t task=ptasks->array[h%ptasks->size];
  int h2=h+1%MAX_SIZE;
  if(!CAS(&anchor, h, s, g, h2, s-1, g))
    goto L2;
  return task;
}

void* owner(void* arg)
{
  int i;
  for(i=0; i<ROUNDS; ++i)
  {
    if(nondet())
    {
      take();
    }
    else
    {
      put(nondet());
    }
  }
}

void* thief(void* arg)
{
  int i;
  for(i=0; i<ROUNDS; ++i)
  {
    steal();
  }
}

int main()
{
  pthread_t t;
  DeIwsq(ROUNDS);
  pthread_create(&t, 0, owner, 0);
  thief(0);
  return 0;
}

