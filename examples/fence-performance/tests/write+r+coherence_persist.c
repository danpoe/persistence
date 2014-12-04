#include <pthread.h>
#include <assert.h>

#define N (1000000000)

#define FENCE asm volatile ("mfence" ::: "memory") // x86 fence (TSO fence)

volatile int x = 0, y = 0, z = 0;

void * thr1(void * arg)
{
for(int i = 0; i < N; ++i){
   	x = 1;
    z = 3;
	while (x != 1) {;}
}
    return 0;
}

void * thr2(void * arg)
{
for(int i = 0; i < N; ++i){
	x = 1;
	y = 2;
}
    return 0;
}

void * thr3(void * arg)
{
for(int i = 0; i < N; ++i){
	y = 3;
	z = 1;
}
    return 0;
}

void * thr4(void * arg)
{
for(int i = 0; i < N; ++i){
   	x = 1;
	z = 3;
	while (x != 1) {;}
}
    return 0;
}

void * thr5(void * arg)
{
for(int i = 0; i < N; ++i){
   	x = 1;
	z = 3;
	while (x != 1) {;}
}
    return 0;
}

void * thr6(void * arg)
{
for(int i = 0; i < N; ++i){
   	x = 1;
	z = 3; 
	while (x != 1) {;}
}
    return 0;
}


int main()
{
    pthread_t t1, t2, t3, t4, t5;
    pthread_create(&t1,NULL,thr1,NULL);
    pthread_create(&t2,NULL,thr2,NULL);
    pthread_create(&t3,NULL,thr3,NULL);
    pthread_create(&t4,NULL,thr4,NULL);
    pthread_create(&t5,NULL,thr5,NULL);
    thr6(NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);
    pthread_join(t5,NULL);
    return 0;

}

