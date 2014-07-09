volatile int flag[9] = {0,0,0,0,0,0,0,0,0}, turn = 0;

int N = 9;

void * thr0(void * arg)
{
  int i = 0;
  int j = 0;
  while (1) {
  L: 	flag[0] = 1;
        while (turn != i) {
   		if (flag[turn] == 0) turn = i;
        }
	flag[i] = 2;
	for (j=0; j<N; j++) {
		if ( j != i && flag[j] == 2) goto L;
	}
        // CS
	flag[i] = 0;
  }
}

void * thr1(void * arg)
{
  int i = 1;
  int j = 0;
  while (1) {
  L: 	flag[0] = 1;
        while (turn != i) {
   		if (flag[turn] == 0) turn = i;
        }
	flag[i] = 2;
	for (j=0; j<N; j++) {
		if ( j != i && flag[j] == 2) goto L;
	}
        // CS
	flag[i] = 0;
  }
}


void * thr2(void * arg)
{
  int i = 2;
  int j = 0;
  while (1) {
  L: 	flag[0] = 1;
        while (turn != i) {
   		if (flag[turn] == 0) turn = i;
        }
	flag[i] = 2;
	for (j=0; j<N; j++) {
		if ( j != i && flag[j] == 2) goto L;
	}
        // CS
	flag[i] = 0;
  }
}

void * thr3(void * arg)
{
  int i = 3;
  int j = 0;
  while (1) {
  L: 	flag[0] = 1;
        while (turn != i) {
   		if (flag[turn] == 0) turn = i;
        }
	flag[i] = 2;
	for (j=0; j<N; j++) {
		if ( j != i && flag[j] == 2) goto L;
	}
        // CS
	flag[i] = 0;
  }
}


void * thr4(void * arg)
{
  int i = 4;
  int j = 0;
  while (1) {
  L: 	flag[0] = 1;
        while (turn != i) {
   		if (flag[turn] == 0) turn = i;
        }
	flag[i] = 2;
	for (j=0; j<N; j++) {
		if ( j != i && flag[j] == 2) goto L;
	}
        // CS
	flag[i] = 0;
  }
}


void * thr5(void * arg)
{
  int i = 5;
  int j = 0;
  while (1) {
  L: 	flag[0] = 1;
        while (turn != i) {
   		if (flag[turn] == 0) turn = i;
        }
	flag[i] = 2;
	for (j=0; j<N; j++) {
		if ( j != i && flag[j] == 2) goto L;
	}
        // CS
	flag[i] = 0;
  }
}

void * thr6(void * arg)
{
  int i = 6;
  int j = 0;
  while (1) {
  L: 	flag[0] = 1;
        while (turn != i) {
   		if (flag[turn] == 0) turn = i;
        }
	flag[i] = 2;
	for (j=0; j<N; j++) {
		if ( j != i && flag[j] == 2) goto L;
	}
        // CS
	flag[i] = 0;
  }
}


void * thr7(void * arg)
{
  int i = 7;
  int j = 0;
  while (1) {
  L: 	flag[0] = 1;
        while (turn != i) {
   		if (flag[turn] == 0) turn = i;
        }
	flag[i] = 2;
	for (j=0; j<N; j++) {
		if ( j != i && flag[j] == 2) goto L;
	}
        // CS
	flag[i] = 0;
  }
}


void * thr8(void * arg)
{
  int i = 8;
  int j = 0;
  while (1) {
  L: 	flag[0] = 1;
        while (turn != i) {
   		if (flag[turn] == 0) turn = i;
        }
	flag[i] = 2;
	for (j=0; j<N; j++) {
		if ( j != i && flag[j] == 2) goto L;
	}
        // CS
	flag[i] = 0;
  }
}

int main()
{
  __CPROVER_ASYNC_0: thr0(0);
  __CPROVER_ASYNC_1: thr1(0);
  __CPROVER_ASYNC_2: thr2(0);
  __CPROVER_ASYNC_3: thr3(0);
  __CPROVER_ASYNC_4: thr4(0);
  __CPROVER_ASYNC_5: thr5(0);
  __CPROVER_ASYNC_6: thr6(0);
  __CPROVER_ASYNC_7: thr7(0);
  __CPROVER_ASYNC_8: thr8(0);
//thr2(0);

return 0;
}

