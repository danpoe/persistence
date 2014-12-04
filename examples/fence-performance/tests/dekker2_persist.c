#include <pthread.h>
#include <assert.h>

#define N (1000000000)

#define FENCE asm volatile ("mfence" ::: "memory") // x86 fence (TSO fence)

volatile int flag[2] = {0,0}, turn = 0;

void * thr0(void * arg)
{
    for(int i = 0; i < N; ++i){

  	flag[0] = 1; FENCE;
  	while (flag[1] == 1){
    		if(!(turn == 0))
      		{
        		flag[0] = 0; FENCE;
       	 		while(!(turn == 0)){;}
        		flag[0] = 1;
      		}
	}
  	turn = 1;
  	flag[0] = 0;
  }
    return 0;
}

void * thr1(void * arg)
{
    for(int i = 0; i < N; ++i){

  	flag[1] = 1; FENCE;
  	while( flag[0] == 1){
    		if(!(turn == 1))
      		{
        		flag[1] = 0; FENCE;
        		while(!(turn == 1)){;}
        		flag[1] = 1;
            }
    }
  turn = 0;
  flag[1] = 0;
 }
    return 0;
}



int main()
{
    pthread_t t1;
    pthread_create(&t1,NULL,thr1,NULL);
    
    thr0(NULL);
    pthread_join(t1,NULL);
    return 0;

}

