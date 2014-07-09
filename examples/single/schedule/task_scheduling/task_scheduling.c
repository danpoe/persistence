volatile int req = 0, turn = 0;

void * thr1(void * arg)
{
  int lreq;
  int lturn;
  while (1) {
  	lreq = req;
	lturn = turn;
  }
}

void * thr2(void * arg)
{
  int lreq;
  int lturn;
  while (1) {
  	lreq = req;
	lturn = turn;
  }
}


void * thr3(void * arg)
{
  int lreq;
  int lturn;
  while (1) {
  	lreq = req;
	lturn = turn;
  }
}

void * thr4(void * arg)
{
  int lreq;
  int lturn;
  while (1) {
  	lreq = req;
	lturn = turn;
  }
}

void * thr5(void * arg)
{
  int lreq;
  int lturn;
  while (1) {
  	lreq = req;
	lturn = turn;
  }
}

void * thr6(void * arg)
{
  int lturn;
  while (1) {
L1:	req = 1;
	lturn = turn;
	if (lturn != 1){ goto L1;}

L2:	req = 2;
	lturn = turn;
	if (lturn != 2){ goto L2;}

	lturn = turn;
 }
}


void * thr7(void * arg)
{
  int lturn;
  while (1) {
L1:	req = 1;
	lturn = turn;
	if (lturn != 1){ goto L1;}

L2:	req = 2;
	lturn = turn;
	if (lturn != 2){ goto L2;}

	lturn = turn;
 }
}


void * thr8(void * arg)
{
  int lturn;
  while (1) {
L1:	req = 1;
	lturn = turn;
	if (lturn != 1){ goto L1;}

L2:	req = 2;
	lturn = turn;
	if (lturn != 2){ goto L2;}

	lturn = turn;
 }
}


void * thr9(void * arg)
{
  int lturn;
  while (1) {
L1:	req = 1;
	lturn = turn;
	if (lturn != 1){ goto L1;}

L2:	req = 2;
	lturn = turn;
	if (lturn != 2){ goto L2;}

	lturn = turn;
 }
}


void * thr10(void * arg)
{
  int lturn;
  while (1) {
L1:	req = 1;
	lturn = turn;
	if (lturn != 1){ goto L1;}

L2:	req = 2;
	lturn = turn;
	if (lturn != 2){ goto L2;}

	lturn = turn;
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
  __CPROVER_ASYNC_6: thr7(0);
  __CPROVER_ASYNC_7: thr8(0);
  __CPROVER_ASYNC_8: thr9(0);
  __CPROVER_ASYNC_9: thr10(0);
//thr2(0);

return 0;
}

