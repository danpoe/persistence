#include <pthread.h>
#include <assert.h>

#define N (100000000)

#define FENCE asm volatile ("mfence" ::: "memory") // x86 fence (TSO fence)


volatile int flag1 = 0, flag2 = 0;

void* thr1(void * arg) {
  for(int i = 0; i < N; ++i){
    flag1 = 1;FENCE;
    while (flag2 >= 3 ) {;} 
    flag1 = 3;FENCE;
    if (flag2 == 1) {
      flag1 = 2;FENCE;
      while (flag2 != 4 ) {;}
    }
    flag1 = 4;FENCE;
     while (flag2 == 3 || flag2 == 2 ) {;}
    flag1 = 0;
  }
    return 0;
}

void* thr2(void * arg) {
  for(int i = 0; i < N; ++i){
    flag2 = 1;FENCE;
    while (flag1 >= 3 ) {;}
    flag2 = 3;FENCE;
    if (flag1 == 1) {
      flag2 = 2;FENCE;
       while (flag1 != 4 ) {;}
    }
    flag2 = 4;FENCE;
     while (flag1 >= 2 ) {;}
    flag2 = 0;
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
