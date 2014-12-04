#include <pthread.h>
#include <assert.h>

#define N (100000000)

#define FENCE asm volatile ("mfence" ::: "memory") // x86 fence (TSO fence)


volatile int b1, b2, x, y;

void * thr1(void * arg)
{
for(int i = 0; i < N; ++i){
  while(1){
      b1 = 1;FENCE;
      x = 1;FENCE;FENCE;
      if(!(y == 0)) {
        b1 = 0;FENCE;
        while(!(y == 0)){;}
      } else {
        y = 1;FENCE;FENCE;
        if(!(x == 1)) {
           b1 = 0;FENCE;
           while (b2 >= 1){;}
           if(!(y != 1)){ goto L9;}
           while(!(y == 0)){;}
        }  else {
            L9:
              goto L11;
        }
     }
 }

L11:
  ;
  y = 0;
  b1 = 0;
}
return 0;
}

void * thr2(void * arg)
{
for(int i = 0; i < N; ++i){
  while(1) {
      b2 = 1;FENCE;
      x = 2;FENCE;FENCE;
      if(!(y == 0)) {
          b2 = 0;FENCE;
          while(!(y == 0)){;}
      } else {
          y = 2;FENCE;FENCE;
          if(!(x == 2)) {
             b2 = 0;FENCE;FENCE;
             while(b1 >= 1){;}
             if(!(y != 2)){goto L9;}
             while(!(y == 0)){;}
          }  else {
            L9:
              goto L11;
          }
      }
  }

L11:
  ;
  y = 0;
  b2 = 0;
}
    return 0;
}


int main()
{
    pthread_t t2;
    pthread_create(&t2,NULL,thr2,NULL);
    thr1(NULL);
    pthread_join(t2,NULL);
    return 0;
}


