volatile int flag0 = 0, flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0, flag5 = 0, flag6 = 0;

void * thr0(void * arg)
{
  while (1) {
        flag0 = 0;
  	flag0 = 1;	
        while (flag1 == 1 || flag2 == 1 || flag3 ==1 || flag4 == 1 || flag5 == 1 || flag6 == 1 ) {;}
	// critical section here
        flag0 = 0;
  }
}

void * thr1(void * arg)
{
  while (1) {
        flag1 = 0;
        if (flag0 != 1) {
  		flag1 = 1;
		if (flag0 != 1) {
			while (flag2 == 1 || flag3 ==1 || flag4 == 1 || flag5 == 1 || flag6 == 1) {;}
			// critical section here
            		flag1 = 0;
		}
        }
 }
}


void * thr2(void * arg)
{
  while (1) {
        flag2 = 0;
        if (flag0 != 1 && flag1 != 1) {
  		flag2 = 1;
		if (flag0 != 1 && flag1 != 1) {
			while (flag3 ==1 || flag4 == 1 || flag5 == 1 || flag6 == 1) {;}
			// critical section here
            		flag2 = 0;
		}
        }
 }
}


void * thr3(void * arg)
{
  while (1) {
        flag3 = 0;
        if (flag0 != 1 && flag1 != 1 && flag2 != 1) {
  		flag3 = 1;
		if (flag0 != 1 && flag1 != 1 && flag2 != 1) {
			while (flag4 == 1 || flag5 == 1 || flag6 == 1) {;}
			// critical section here
            		flag3 = 0;
		}
        }
 }
}

void * thr4(void * arg)
{
  while (1) {
        flag4 = 0;
        if (flag0 != 1 && flag1 != 1 && flag2 != 1 && flag3 !=1 ) {
  		flag4 = 1;
		if (flag0 != 1 && flag1 != 1 && flag2 != 1 && flag3 !=1) {
			while (flag5 == 1 || flag6 == 1) {;}               
			// critical section here
            		flag4 = 0;
		}
        }
 }
}

void * thr5(void * arg)
{
  while (1) {
        flag5 = 0;
        if (flag0 != 1 && flag1 != 1 && flag2 != 1 && flag3 !=1 && flag4 !=1  ) {
  		flag5 = 1;
		if (flag0 != 1 && flag1 != 1 && flag2 != 1 && flag3 !=1 && flag4 !=1 ) {           
			while (flag6 == 1) {;} 	
			// critical section here
            		flag5 = 0;
		}
        }
 }
}


void * thr6(void * arg)
{
  while (1) {
        flag6 = 0;
        if (flag0 != 1 && flag1 != 1 && flag2 != 1 && flag3 !=1 && flag4 !=1 && flag5 !=1  ) {
  		flag6 = 1;
		if (flag0 != 1 && flag1 != 1 && flag2 != 1 && flag3 !=1 && flag4 !=1 && flag5 !=1) {           
			// critical section here
            		flag6 = 0;
		}
        }
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
return 0;
}

