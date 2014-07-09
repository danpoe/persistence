volatile int c = 1, x = 0, y = 0;

void * w1(void * arg)
{
  int c0;
  while (1) {
 	c0 = c;
	c = c0 + 1;
	x = 1;
	y = 1;
        c = c0 + 2;
  }
}

void * w2(void * arg)
{
  int c0;
  while (1) {
 	c0 = c;
	c = c0 + 1;
	x = 1;
	y = 1;
        c = c0 + 2;
  }
}


void * w3(void * arg)
{
  int c0;
  while (1) {
 	c0 = c;
	c = c0 + 1;
	x = 1;
	y = 1;
        c = c0 + 2;
  }
}


void * r1(void * arg)
{
  int c0, x1, y1;
  while (1) {
 L1:	c0 = c;
        while (c0 % 2) goto L1;
	x1 = x;
	y1 = y;
	if (c0 != c) goto L1;
  }
}

void * r2(void * arg)
{
  int c0, x1, y1;
  while (1) {
 L1:	c0 = c;
        while (c0 % 2) goto L1;
	x1 = x;
	y1 = y;
	if (c0 != c) goto L1;
  }
}


void * r3(void * arg)
{
  int c0, x1, y1;
  while (1) {
 L1:	c0 = c;
        while (c0 % 2) goto L1;
	x1 = x;
	y1 = y;
	if (c0 != c) goto L1;
  }
}


void * r4(void * arg)
{
  int c0, x1, y1;
  while (1) {
 L1:	c0 = c;
        while (c0 % 2) goto L1;
	x1 = x;
	y1 = y;
	if (c0 != c) goto L1;
  }
}


void * r5(void * arg)
{
  int c0, x1, y1;
  while (1) {
 L1:	c0 = c;
        while (c0 % 2) goto L1;
	x1 = x;
	y1 = y;
	if (c0 != c) goto L1;
  }
}


int main()
{
  __CPROVER_ASYNC_0: w1(0);
  __CPROVER_ASYNC_1: w2(0);
  __CPROVER_ASYNC_2: w3(0); 
  __CPROVER_ASYNC_3: r1(0);
  __CPROVER_ASYNC_4: r2(0);
  __CPROVER_ASYNC_5: r3(0);
  __CPROVER_ASYNC_6: r4(0);
  __CPROVER_ASYNC_7: r5(0);
return 0;
}
