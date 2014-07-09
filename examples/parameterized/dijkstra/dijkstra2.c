volatile int flag0 = 0, flag1 = 0, turn = 0;

void * thr0(void * arg)
{
  while (1) {
  L: 	flag0 = 1;
        while (turn != 0) {
		if (flag1 == 0) turn = 0;
        }
	flag0 = 2;
	if (flag1 == 2) goto L;
	flag0 = 0;
  }
}

void * thr1(void * arg)
{
  while (1) {
  L: 	flag1 = 1;
        while (turn != 1) {
		if (flag0 == 0) turn = 1;
        }
	flag1 = 2;
	if (flag0 == 2) goto L;
	flag1 = 0;
 }
}


int main()
{
  __CPROVER_ASYNC_0: thr0(0);
  __CPROVER_ASYNC_1: thr1(0);
//thr2(0);

return 0;
}

