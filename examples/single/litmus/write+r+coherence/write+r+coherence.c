volatile int x = 0, y = 0, z = 0;

void * thr1(void * arg)
{
while (1) {
   	x = 1;
	z = 3;
	while (x != 1) {;}
}
}

void * thr2(void * arg)
{
while (1) {
	x = 1;
	y = 2;
}
}

void * thr3(void * arg)
{
while (1) {
	y = 3;
	z = 1;
}
}

void * thr4(void * arg)
{
while (1) {
   	x = 1;
	z = 3;
	while (x != 1) {;}
}
}

void * thr5(void * arg)
{
while (1) {
   	x = 1;
	z = 3;
	while (x != 1) {;}
}
}

void * thr6(void * arg)
{
while (1) {
   	x = 1;
	z = 3;
	while (x != 1) {;}
}
}


int main()
{
  __CPROVER_ASYNC_0: thr1(0);
  __CPROVER_ASYNC_1: thr2(0);
  __CPROVER_ASYNC_2: thr3(0);
  __CPROVER_ASYNC_3: thr4(0);
  __CPROVER_ASYNC_4: thr5(0);
  __CPROVER_ASYNC_5: thr6(0);
  return 0;
}

