/* The Implementation of the Cilk-5 Multithreaded Language
 * M. Frigo, C. E. Leiserson, K. H. Randall
 * PLDI'98 
 */
#ifdef PTHREAD
#include <pthread.h>
#endif

typedef int lock_t;

#define FAILURE 0
#define SUCCESS 1

#define ROUNDS 10 
#define capacity 10

volatile lock_t L = 0;
void lock() { 
  __CPROVER_atomic_begin();
  __asm__ __volatile__ ("mfence": : :"memory");
  __CPROVER_assume(L==0); 
  L=1; 
  __asm__ __volatile__ ("mfence": : :"memory");
  __CPROVER_atomic_end();
}
void unlock() {
  __CPROVER_atomic_begin(); 
  __asm__ __volatile__ ("mfence": : :"memory");
  __CPROVER_assume(L==1); 
  L=0; 
  __asm__ __volatile__ ("mfence": : :"memory");
  __CPROVER_atomic_end();
}

int counter = 0;

volatile int T = capacity/2;
volatile int H = capacity/2;

void push() 
{
  T++;
  __CPROVER_atomic_begin();
  counter++;
  __CPROVER_atomic_end();
}

int pop()
{
  T--;
  //__asm__ __volatile__ ("mfence": : :"memory");
  if(H>T)
  {
    T++;
    lock();
    T--;
    if(H>T)
    {
      T++;
      unlock();
      return FAILURE;
    }
    unlock();
  }
  __CPROVER_atomic_begin();
  counter--;
  __CPROVER_atomic_end();
  return SUCCESS;
}

int steal()
{
  lock();
  H++;
  //__asm__ __volatile__ ("mfence": : :"memory");
  if(H>T)
  {
    H--;
    unlock();
    return FAILURE;
  }
  unlock();
  __CPROVER_atomic_begin();
  counter--;
  __CPROVER_atomic_end();
  return SUCCESS;
}

void* owner(void * arg)
{
  int i;
  for(i=0; i<ROUNDS; ++i)
  {
    if(nondet())
    {
      pop();
    }
    else
    {
      push();
    }
  }
}

void* thief(void * arg)
{
  int i;
  for(i=0; i<ROUNDS; ++i)
  {
    steal();
  }
}

int main()
{
#ifdef PTHREAD
  pthread_t pt;
  pthread_create(&pt, 0, owner, 0);
  thief(0);
  pthread_join(pt, 0);
#else
  __CPROVER_ASYNC_0: owner(0);
  thief(0);
#endif
  assert(counter>=0);
  return 0;
}

