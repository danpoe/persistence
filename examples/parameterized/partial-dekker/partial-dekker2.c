volatile int flag0 = 0, flag1 = 0, turn = 0;

void * thr0(void * arg)
{
  while (1) {
  	flag0 = 1;
  	while(flag1 >= 1){
    		if(!(turn == 0))
      		{
        		flag0 = 0;
       	 		while(!(turn == 0)){;}
        		flag0 = 1;
      		}
	}
  	turn = 1;
  	flag0 = 0;
  }
}

void * thr1(void * arg)
{
  while (1) {
  	flag1 = 1;
  	while(flag0 >= 1){
    		if(!(turn == 1))
      		{
        		flag1 = 0;
        		while(!(turn == 1)){;}
        		flag1 = 1;
      }}
  turn = 1;
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

