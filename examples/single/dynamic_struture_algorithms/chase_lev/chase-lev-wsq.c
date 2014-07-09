/* Dynamic Circular Work-Stealing Deque
 * D. Chase, Y. Lev
 * SPAA'05
 */
#include <pthread.h>

#define ROUNDS 10

#define Empty 0
#define Abort -1
#define Object 1

volatile int top=0;
volatile int bottom=0;

_Bool inline casTop(int oldVal, int newVal)
{
  _Bool preCond=0;
  __CPROVER_atomic_begin();
  preCond=(top==oldVal);
  if(preCond)
    top=newVal;
  __CPROVER_atomic_end();
  return preCond;
}

void pushBottom(/* Object o */) 
{
  int b=bottom;
  int t=top;
  // CircularArray a=this.activeArray;
  int size=b-t;
  if(size >= nondet() /*a.size()*/ -1)
  {
    int grow=b+t; /* a=a.grow(b,t); */
    // this.activeArray=a;
  }
  // a.put(b, o)
  int put=b;
  bottom=b+1;
}

int popBottom()
{
  int b=bottom;
  // CircularArray a=this.activeArray;
  b=b-1;
  bottom=b;
  int t=top;
  int size=b-t;
  if(size<0) {
    bottom=t;
    return Empty;
  }
  // Object o=a.get(b);
  int o=Object+b;
  if(size>0)
    return o;
  if(!casTop(t, t+1))
    o=Empty;
  bottom=t+1;
  return o;
}

int steal()
{
  int t=top;
  int b=bottom;
  // CircularArray a=this.activeArray;
  int size=b-t;
  if(size<=0)
    return Empty;
  // Object o=a.get(t)
  int o=Object+t;
  if(!casTop(t,t+1))
    return Abort;
  return o;
}

void * owner(void * arg)
{
  int i;
  for(i=0; i<ROUNDS; ++i)
  {
    if(nondet())
    {
      popBottom();
    }
    else
    {
      pushBottom();
    }
  }
}

void * thief(void * arg)
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
  pthread_create(&pt, 0, owner, 0);
  thief(0);
  return 0;
}

