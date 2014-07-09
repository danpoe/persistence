volatile int b[3]={0,0,0}, x, y;


void * thr1(void * arg)
{
while (1) {
  while(1){
      b[0] = 1;
      x = 1;
      if(!(y == 0)) {
        b[0] = 0;
        while(!(y == 0)){;}
      } else {
        y = 1;
        if(!(x == 1)) {
           b[0] = 0;
           while (b[1] >= 1 || b[2] >= 1){;}
           if(!(y != 1)){ goto L9;}
           while(!(y == 0)){;}
        }  else {
            L9:
              goto L11;
        }
     }
 }

L11:
  ;
  y = 0;
  b[0] = 0;
}

}


void * thr2(void * arg)
{
while (1) {
  while(1){
      b[1] = 1;
      x = 2;
      if(!(y == 0)) {
        b[1] = 0;
        while(!(y == 0)){;}
      } else {
        y = 2;
        if(!(x == 2)) {
           b[1] = 0;
           while (b[2] >= 1 || b[0] >= 1){;}
           if(!(y != 2)){ goto L9;}
           while(!(y == 0)){;}
        }  else {
            L9:
              goto L11;
        }
     }
 }

L11:
  ;
  y = 0;
  b[1] = 0;
}

}

void * thr3(void * arg)
{
while (1) {
  while(1) {
      b[2] = 1;
      x = 3;
      if(!(y == 0)) {
          b[2] = 0;
          while(!(y == 0)){;}
      } else {
          y = 3;
          if(!(x == 3)) {
             b[2] = 0;
             while(b[1] >= 1 || b[0] >= 1){;}
             if(!(y != 3)){goto L9;}
             while(!(y == 0)){;}
          }  else {
            L9:
              goto L11;
          }
      }
  }

L11:
  ;
  y = 0;
  b[2] = 0;
}

}


int main()
{
  __CPROVER_ASYNC_1: thr1(0);
  __CPROVER_ASYNC_2: thr2(0);
   __CPROVER_ASYNC_3: thr3(0);
return 0;
}


