#include <pthread.h>
#include <assert.h>

#define N (1000000000)

#define FENCE asm volatile ("mfence" ::: "memory") // x86 fence (TSO fence)


volatile int c1 = 0, c2 = 0, c3 = 0, n1= 0, n2 = 0, n3 = 0;

void * thr1(void * arg)
{
  int max;
  
  for(int i = 0; i < N; ++i){

    c1 = 1;FENCE;
	if (c2 < c3) {
	  max = c3;
        } else {
	  max = c2;
        }
	n1 = max + 1;
	c1 = 0;FENCE;
       
	while (c2 != 0) {;}
	while (n2 > 0 && n2 < n1) {;} 
	
	while (c3 != 0) {;}
	while (n3 > 0 && n3 < n1) {;} 
	
	n1 = 0;
  }
    return 0;
}

void * thr2(void * arg)
{
  int max;
  
  for(int i = 0; i < N; ++i){

	c2 = 1;FENCE;
	if (c1 < c3) {
	  max = c3;
        } else {
	  max = c1;
        }
	n2 = max + 1;
	c2 = 0;FENCE;
       
	while (c1 != 0) {;}
	while (n1 > 0 && n1 <= n2) {;} 
	
	while (c3 != 0) {;}
	while (n3 > 0 && n3 < n2) {;} 
	
	n2 = 0;
  }
    return 0;
}


void * thr3(void * arg)
{
  int max;
 
  for(int i = 0; i < N; ++i){

	c3 = 1;FENCE;
	if (c1 < c2)  {
	  max = c2;
        } else {
	  max = c1;
        }
	n3 = max + 1;
	c3 = 0;FENCE;
       
	while (c1 != 0) {;}
	while (n1 > 0 && n1 <= n3) {;} 
	
	while (c2 != 0) {;}
	while (n2 > 0 && n2 <= n3) {;} 
	
	n3 = 0;
  }
    return 0;
}

int main()
{
    pthread_t t2, t3;
    pthread_create(&t2,NULL,thr2,NULL);
    pthread_create(&t3,NULL,thr3,NULL);
    thr1(NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    
    return 0;
}

