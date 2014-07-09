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

typedef int task_t;

typedef struct {
int size;
task_t* array;
} array_with_size_t;

/* shared structures */
int head;
int tail;
array_with_size_t* tasks;

#ifndef MALLOC
task_t shared_tasks[ROUNDS];
size_t counter=0;
array_with_size_t shared_arrays[ROUNDS];
size_t counter_arrays=0;
#endif

task_t* new_task_array(int size) {
#ifdef MALLOC
  return (task_t*)malloc(sizeof(task_t));
#else
  if(counter+(size_t)size<ROUNDS)
  {
    const size_t cnt=counter;
    counter+=(size_t)size;
    return shared_tasks+cnt;
  }
  else
    return NULL;
#endif
}

array_with_size_t* new_array_with_size() {
#ifdef MALLOC
  return (array_with_size_t*)malloc(sizeof(array_with_size_t));
#else
  if(counter<ROUNDS)
    return shared_arrays+counter_arrays++;
  else
    return NULL;
#endif
}

/* constructor */
void FifoIwsq(int size) {
  head=0;
  tail=0;
  tasks=new_array_with_size();
  tasks->array=new_task_array(size);
}

/* CAS */
_Bool CAS(int* loc, int old_val, int new_val) {
  _Bool preCond;
  __CPROVER_atomic_begin();
  preCond=(*loc==old_val);
  if(preCond) {
    *loc=new_val;
  }
  __CPROVER_atomic_end();
  return preCond;
}

void expand() {
  int size=tasks->size;
  array_with_size_t* new_tasks=new_array_with_size();
  new_tasks->array=new_task_array(size*2);
  new_tasks->size=2*size;
  int i;
  for(i=head; i<tail; i++)
    new_tasks->array[i%new_tasks->size]=tasks->array[i%tasks->size];
  tasks=new_tasks;
}

void put(task_t task) {
L1:; 
  int h=head;
  int t=tail;
  if(t==h+tasks->size) {
    expand();
    goto L1;
  }
  tasks->array[t%tasks->size]=task;
  tail=t+1;
}

task_t take() {
  int h=head;
  int t=tail;
  if(h==t)
    return EMPTY;
  task_t task=tasks->array[h%tasks->size];
  head=h+1;
  return task;
}

task_t steal() {
L2:;
  int h=head;
  int t=tail;
  if(h==t)
    return EMPTY;
  array_with_size_t* ptasks=tasks;
  task_t task=ptasks->array[h%ptasks->size];
  if(!CAS(&head, h, h+1))
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
  pthread_t pt;
  FifoIwsq(ROUNDS);
  pthread_create(&pt, 0, owner, 0);
  thief(0);
  return 0;
}

