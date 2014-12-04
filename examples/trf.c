#include <pthread.h>
#include <assert.h>

volatile int flag[2] = {0,0};

void * thr0(void * arg)
{
  	flag[0] = 0;
  	if (flag[1] == 1){
	}
    return 0;
}

void * thr1(void * arg)
{
    flag[1] = 0;
    if (flag[0] == 1){
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

