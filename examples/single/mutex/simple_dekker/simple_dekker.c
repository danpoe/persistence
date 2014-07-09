volatile int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;

void * thr1(void * arg)
{
while (1) {
L0:   	flag1 = 1;
  	if (flag2 == 1 || flag3 == 1 || flag4 == 1){
    		flag1 = 0;
		goto L0;
	}
//CS
	flag1 = 0;
}
}

void * thr2(void * arg)
{
while (1) {
L1:   	flag2 = 1;
  	if (flag1 == 1 || flag3 == 1 || flag4 == 1){
    		flag2 = 0;
		goto L1;
	}
//CS
	flag2 = 0;
}
}

void * thr3(void * arg)
{
while (1) {
L1:   	flag3 = 1;
  	if (flag1 == 1 || flag2 == 1 || flag4 == 1){
    		flag3 = 0;
		goto L1;
	}
//CS
	flag3 = 0;
}
}

void * thr4(void * arg)
{
while (1) {
L1:   	flag4 = 1;
  	if (flag1 == 1 || flag2 == 1 || flag3 == 1){
    		flag4 = 0;
		goto L1;
	}
//CS
	flag4 = 0;
}
}

int main()
{
  __CPROVER_ASYNC_0: thr1(0);
  __CPROVER_ASYNC_1: thr2(0);
  __CPROVER_ASYNC_2: thr3(0);
  __CPROVER_ASYNC_3: thr4(0);

  return 0;
}

