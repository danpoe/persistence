#include <pthread.h>
#include <assert.h>

#define N (1000000000)

#define FENCE asm volatile ("mfence" ::: "memory") // x86 fence (TSO fence)

volatile int ccf = 0, buf = 0;
 
void* writer(void * arg) {
   int ccf_writer;
    for(int i = 0; i < N; ++i){
   ccf_writer = ccf;
   ccf = ccf_writer + 1;
   buf = 1;
   ccf = ccf_writer + 2;
    }
    return 0;
}

void* write_reader(void * arg) {
   int ccf_reader, ccf_begin, ccf_end, value;

    for(int i = 0; i < N; ++i){
   ccf_reader = ccf;
   ccf = ccf_reader + 1;
   buf = 1;
   ccf = ccf_reader + 2;

L0:   
   ccf_begin = ccf;
   value = buf;
   ccf_end = ccf;

   if (ccf_end != ccf_begin || ccf_begin == 1){ 
     goto L0;}

    }
    return 0;

} 
   

int main()
{
    pthread_t t1;
    pthread_create(&t1,NULL,writer,NULL);
    write_reader(NULL);
    pthread_join(t1,NULL);
    return 0;

}
