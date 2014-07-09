volatile int _lock = 0;
volatile int instance = 0;

void lock(){
  __CPROVER_atomic_begin();
  __CPROVER_assume(_lock==0);
  _lock = 1;
  __CPROVER_atomic_end();
};

void unlock(){
  _lock = 0;
};



void * instance1(void * arg)
{
for (;;) {
  if(instance == 0){
     lock();
     if(instance == 0){
        instance = 1;
     }
     unlock();
   }
   instance;
  }
}

void * instance2(void * arg)
{
for (;;) {
  if(instance == 0){
     lock();
     if(instance == 0){
        instance = 1;
     }
     unlock();
   }
   instance;
  }
}

int main()
{
  __CPROVER_ASYNC_0: instance1(0);
  __CPROVER_ASYNC_1: instance2(0);
return 0;
}

