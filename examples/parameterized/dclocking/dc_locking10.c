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

void * instance3(void * arg)
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

void * instance4(void * arg)
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

void * instance5(void * arg)
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

void * instance6(void * arg)
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

void * instance7(void * arg)
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

void * instance8(void * arg)
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

void * instance9(void * arg)
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


void * instance10(void * arg)
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
  __CPROVER_ASYNC_2: instance3(0);
  __CPROVER_ASYNC_3: instance4(0);
  __CPROVER_ASYNC_4: instance5(0);
  __CPROVER_ASYNC_5: instance6(0);
  __CPROVER_ASYNC_6: instance7(0);
  __CPROVER_ASYNC_7: instance8(0);
  __CPROVER_ASYNC_8: instance9(0);
  __CPROVER_ASYNC_9: instance10(0);
return 0;
}

