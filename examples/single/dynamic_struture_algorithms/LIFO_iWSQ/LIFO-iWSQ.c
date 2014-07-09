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
  int tail;
  int tag;
} anchor_t;

/* shared structures */
anchor_t anchor;
int capacity;
task_t* tasks;

task_t shared_tasks[ROUNDS];
size_t counter=0;

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

/* constructor */
void LifoIwsq(int size) {
  anchor.tail=0;
  anchor.tag=0;
  capacity=size;
  tasks=new_task_array(size);
}

/* CAS */
_Bool CAS(anchor_t* anc, int t_old, int g_old, int t_new, int g_new) {
  _Bool preCond;
  __CPROVER_atomic_begin();
  preCond=(anc->tail==t_old && anc->tag==g_old);
  if(preCond) {
    anc->tail=t_new;
    anc->tag=g_new;
  }
  __CPROVER_atomic_end();
  return preCond;
}

void expand() {
  task_t* new_tasks=new_task_array(capacity);
  int i;
  for(i=0; i<capacity; i++)
    new_tasks[i]=tasks[i];
  tasks=new_tasks;
  capacity=capacity*2;
}

void put(task_t task) {
L1: 
  __CPROVER_atomic_begin();
  int t=anchor.tail;
  int g=anchor.tag;
  __CPROVER_atomic_end();
  if(t==capacity) {
    expand();
    goto L1;
  }
  tasks[t]=task;
  __CPROVER_atomic_begin();
  anchor.tail=t+1;
  anchor.tag=g+1;
  __CPROVER_atomic_end();
}

task_t take() {
  __CPROVER_atomic_begin();
  int t=anchor.tail;
  int g=anchor.tag;
  __CPROVER_atomic_end();
  if(t==0)
    return EMPTY;
  task_t task=tasks[t-1];
  __CPROVER_atomic_begin();
  anchor.tail=t-1;
  anchor.tag=g;
  __CPROVER_atomic_end();
  return task;
}

task_t steal() {
L2:
  __CPROVER_atomic_begin();
  int t=anchor.tail;
  int g=anchor.tag;
  __CPROVER_atomic_end();
  if(t==0)
    return EMPTY;
  task_t* ptasks=tasks;
  task_t task=ptasks[t-1];
  if(!CAS(&anchor, t, g, t-1, g))
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
  LifoIwsq(ROUNDS);
  pthread_create(&t, 0, owner, 0);
  thief(0);
  return 0;
}

