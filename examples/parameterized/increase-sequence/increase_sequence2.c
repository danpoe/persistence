volatile int msg = 0;

void * thr1(void * arg)
{
  int i=1;
  while (i<10) {
	// here we remove the non-deterministic loop   	
	msg = 1;
  }
}

void * thr2(void * arg)
{
  int val1, val2;
  msg  = 0;
  val1= msg;
  val2 = msg;
}


int main()
{
  __CPROVER_ASYNC_0: thr1(0);
  __CPROVER_ASYNC_1: thr2(0);


//thr2(0);

return 0;
}

