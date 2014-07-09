volatile int b1, b2, x, y;

void * thr1(void * arg)
{
while (1) {
  while(1){
      b1 = 1;
      x = 1;
      if(!(y == 0)) {
        b1 = 0;
        while(!(y == 0)){;}
      } else {
        y = 1;
        if(!(x == 1)) {
           b1 = 0;
           while (b2 >= 1){;}
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
  b1 = 0;
}

}

void * thr2(void * arg)
{
while (1) {
  while(1) {
      b2 = 1;
      x = 2;
      if(!(y == 0)) {
          b2 = 0;
          while(!(y == 0)){;}
      } else {
          y = 2;
          if(!(x == 2)) {
             b2 = 0;
             while(b1 >= 1){;}
             if(!(y != 2)){goto L9;}
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
  b2 = 0;
}

}


int main()
{
  __CPROVER_ASYNC_1: thr1(0);
  __CPROVER_ASYNC_2: thr2(0);
return 0;
}


