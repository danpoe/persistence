volatile int x = 1, tmp1 = 0, tmp2 = 0;

void * thr2(void * arg)
{
  while (1) {
        __CPROVER_atomic_begin();
        tmp2 = x;
        x = tmp2 - 1;
        __asm volatile ("mfence":::"memory");
	__CPROVER_atomic_begin();
        if (tmp2 > 0) {
           x = 1;
        } else {
            while (x <= 0) {;}
        } 
  }
}

void * thr1(void * arg)
{
  while (1) {
        __CPROVER_atomic_begin();
        tmp1 = x;
        x = tmp1 - 1;
        __asm volatile ("mfence":::"memory");
	__CPROVER_atomic_begin();
        if (tmp1 > 0) {
           x = 1;
        } else {
            while (x <= 0) {;}
        } 
  }
}




int main()
{
  __CPROVER_ASYNC_0: thr1(0);
  __CPROVER_ASYNC_1: thr2(0);
  
return 0;
}

