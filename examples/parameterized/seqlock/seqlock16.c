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


void * r6(void * arg)
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


void * r7(void * arg)
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



void * r8(void * arg)
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


void * r9(void * arg)
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


void * r10(void * arg)
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


void * r11(void * arg)
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


void * r12(void * arg)
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


void * r13(void * arg)
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
  __CPROVER_ASYNC_8: r6(0);
  __CPROVER_ASYNC_9: r7(0);
 __CPROVER_ASYNC_10: r8(0);
  __CPROVER_ASYNC_11: r9(0);

 __CPROVER_ASYNC_12: r10(0);
  __CPROVER_ASYNC_13: r11(0);

 __CPROVER_ASYNC_14: r12(0);
  __CPROVER_ASYNC_15: r13(0);
return 0;
}
