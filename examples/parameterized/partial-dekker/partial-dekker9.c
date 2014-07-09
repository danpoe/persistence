volatile int flag[9] = {0,0,0,0,0,0,0,0,0}, turn = 0;

void * thr0(void * arg)
{
  while (1) {
  	flag[0] = 1;
  	while(flag[1] >= 1 || flag[8] >= 1){
    		if(!(turn == 0))
      		{
        		flag[0] = 0;
       	 		while(!(turn == 0)){;}
        		flag[0] = 1;
      		}
	}
  	turn = 1;
  	flag[0] = 0;
  }
}

void * thr1(void * arg)
{
  while (1) {
  	flag[1] = 1;
  	while(flag[0] >= 1 || flag[2] >= 1){
    		if(!(turn == 1))
      		{
        		flag[1] = 0;
        		while(!(turn == 1)){;}
        		flag[1] = 1;
      }}
  turn = 2;
  flag[1] = 0;
 }
}


void * thr2(void * arg)
{
  while (1) {
  	flag[2] = 1;
  	while(flag[1] >= 1 || flag[3] >= 1){
    		if(!(turn == 2))
      		{
        		flag[2] = 0;
        		while(!(turn == 2)){;}
        		flag[2] = 1;
      }}
  turn = 3;
  flag[2] = 0;
 }
}

void * thr3(void * arg)
{
  while (1) {
  	flag[3] = 1;
  	while(flag[2] >= 1 || flag[4] >= 1){
    		if(!(turn == 3))
      		{
        		flag[3] = 0;
        		while(!(turn == 3)){;}
        		flag[3] = 1;
      }}
  turn = 4;
  flag[3] = 0;
 }
}

void * thr4(void * arg)
{
  while (1) {
  	flag[4] = 1;
  	while(flag[3] >= 1 || flag[5] >= 1){
    		if(!(turn == 4))
      		{
        		flag[4] = 0;
        		while(!(turn == 4)){;}
        		flag[4] = 1;
      }}
  turn = 5;
  flag[4] = 0;
 }
}


void * thr5(void * arg)
{
  while (1) {
  	flag[5] = 1;
  	while(flag[4] >= 1 || flag[6] >= 1){
    		if(!(turn == 5))
      		{
        		flag[5] = 0;
        		while(!(turn == 5)){;}
        		flag[5] = 1;
      }}
  turn = 6;
  flag[5] = 0;
 }
}

void * thr6(void * arg)
{
  while (1) {
  	flag[6] = 1;
  	while(flag[5] >= 1 || flag[7] >= 1){
    		if(!(turn == 6))
      		{
        		flag[6] = 0;
        		while(!(turn == 6)){;}
        		flag[6] = 1;
      }}
  turn = 7;
  flag[6] = 0;
 }
}

void * thr7(void * arg)
{
  while (1) {
  	flag[7] = 1;
  	while(flag[6] >= 1 || flag[8] >= 1){
    		if(!(turn == 7))
      		{
        		flag[7] = 0;
        		while(!(turn == 7)){;}
        		flag[7] = 1;
      }}
  turn = 8;
  flag[7] = 0;
 }
}

void * thr8(void * arg)
{
  while (1) {
  	flag[8] = 1;
  	while(flag[7] >= 1 || flag[0] >= 1){
    		if(!(turn == 8))
      		{
        		flag[8] = 0;
        		while(!(turn == 8)){;}
        		flag[8] = 1;
      }}
  turn = 0;
  flag[8] = 0;
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

