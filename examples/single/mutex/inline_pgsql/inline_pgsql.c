volatile _Bool latch1 = 1, latch2 = 0;
volatile _Bool flag1 = 1, flag2 = 0;


void* worker_1(void* arg) 
{  
  while(!latch1) {} 

  for(;;) 
  { 
    
    latch1 = 0;
    if(flag1) 
    { 
      flag1 = 0;
      flag2 = 1;
      latch2 = 1;
    } 

    while(!latch1) {} 
  } 
}

void* worker_2(void* arg)
{
  while(!latch2) {} 

  for(;;)
  {
   
    latch2 = 0;
    
    if(flag2)
    {
      flag2 = 0;
      flag1 = 1;
      latch1 = 1;
    }

    while(!latch2) {}
  }
}

void* worker_3(void* arg)
{
  while(!latch2) {} 

  for(;;)
  {
   
    latch2 = 0;
    
    if(flag2)
    {
      flag2 = 0;
      flag1 = 1;
      latch1 = 1;
    }

    while(!latch2) {}
  }
}

void* worker_4(void* arg)
{
  while(!latch2) {} 

  for(;;)
  {
   
    latch2 = 0;
    
    if(flag2)
    {
      flag2 = 0;
      flag1 = 1;
      latch1 = 1;
    }

    while(!latch2) {}
  }
}

void* worker_5(void* arg)
{
  while(!latch2) {} 

  for(;;)
  {
   
    latch2 = 0;
    
    if(flag2)
    {
      flag2 = 0;
      flag1 = 1;
      latch1 = 1;
    }

    while(!latch2) {}
  }
}

void* worker_6(void* arg)
{
  while(!latch2) {} 

  for(;;)
  {
   
    latch2 = 0;
    
    if(flag2)
    {
      flag2 = 0;
      flag1 = 1;
      latch1 = 1;
    }

    while(!latch2) {}
  }
}

void* worker_7(void* arg)
{
  while(!latch2) {} 

  for(;;)
  {
   
    latch2 = 0;
    
    if(flag2)
    {
      flag2 = 0;
      flag1 = 1;
      latch1 = 1;
    }

    while(!latch2) {}
  }
}

void* worker_8(void* arg)
{
  while(!latch2) {} 

  for(;;)
  {
   
    latch2 = 0;
    
    if(flag2)
    {
      flag2 = 0;
      flag1 = 1;
      latch1 = 1;
    }

    while(!latch2) {}
  }
}
int main()
{
__CPROVER_ASYNC_1: worker_1(0);
__CPROVER_ASYNC_2: worker_2(0);
__CPROVER_ASYNC_3: worker_3(0);
__CPROVER_ASYNC_4: worker_4(0);
__CPROVER_ASYNC_5: worker_5(0);
__CPROVER_ASYNC_6: worker_6(0);
__CPROVER_ASYNC_7: worker_7(0);
__CPROVER_ASYNC_8: worker_8(0);
  return 0;
}

