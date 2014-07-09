volatile int mem[5]={0,0,0,0,0}, lock=5;

void * thr0(void * arg)
{
  int i=0, p=0, lock_; 
  while (1) {
	mem[i] = 1;
	lock_ = lock;
	__CPROVER_atomic_begin();
	if (lock == lock_)
		lock = p;	
	__CPROVER_atomic_end();
	p = lock_;
	while (mem[p] != 0) {;}
	mem[i] = 0;
	i = p;
  }
}


void * thr1(void * arg)
{
  int i=1, p=1, lock_; 
  while (1) {
	mem[i] = 1;
	lock_ = lock;
	__CPROVER_atomic_begin(); 
	if (lock == lock_)
		lock = p;	
	__CPROVER_atomic_end();
	p = lock_;
	while (mem[p] != 0) {;}
	mem[i] = 0;
	i = p;
  }
}

void * thr2(void * arg)
{
  int i=2, p=2, lock_; 
  while (1) {
	mem[i] = 1;
	lock_ = lock;
	__CPROVER_atomic_begin(); 
	if (lock == lock_)
		lock = p;	
	__CPROVER_atomic_end();
	p = lock_;
	while (mem[p] != 0) {;}
	mem[i] = 0;
	i = p;
  }
}

void * thr3(void * arg)
{
  int i=3, p=3, lock_; 
  while (1) {
	mem[i] = 1;
	lock_ = lock;
	__CPROVER_atomic_begin(); 
	if (lock == lock_)
		lock = p;	
	__CPROVER_atomic_end();
	p = lock_;
	while (mem[p] != 0) {;}
	mem[i] = 0;
	i = p;
  }
}


int main()
{
  __CPROVER_ASYNC_0: thr0(0);
  __CPROVER_ASYNC_1: thr1(0);
  __CPROVER_ASYNC_2: thr2(0);
  __CPROVER_ASYNC_3: thr3(0);


//thr2(0);

return 0;
}

