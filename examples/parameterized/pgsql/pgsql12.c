volatile _Bool latch = 0;
volatile _Bool work_to_do = 0;
volatile _Bool existWaiter = 0;

void* waiter1(void* arg) 
{  
  for(;;) 
  {   
    // ResetLatch();
    latch = 0;

    //check work;
    if(work_to_do) 
    { 
      //do work here; 
      work_to_do = 0;
    } 

    //WaitLatch();
    while(!latch) {} 

  } 
}

void* waiter2(void* arg) 
{  
  for(;;) 
  {   
    // ResetLatch();
    latch = 0;

    //check work;
    if(work_to_do) 
    { 
      //do work here; 
      work_to_do = 0;
    } 

    //WaitLatch();
    while(!latch) {} 

  } 
}

void* waker1(void* arg)
{
  
  for(;;)
  {
    work_to_do = 1;

    //SetLatch();
    if(latch == 0)
    {
      latch = 1;
    } else {
  	//return in SetLatch();
    }
  }

}


void* waker2(void* arg)
{
  
  for(;;)
  {
    work_to_do = 1;

    //SetLatch();
    if(latch == 0)
    {
      latch = 1;
    } else {
  	//return in SetLatch();
    }
  }

}

void* waker3(void* arg)
{
  
  for(;;)
  {
    work_to_do = 1;

    //SetLatch();
    if(latch == 0)
    {
      latch = 1;
    } else {
  	//return in SetLatch();
    }
  }

}

void* waker4(void* arg)
{
  
  for(;;)
  {
    work_to_do = 1;

    //SetLatch();
    if(latch == 0)
    {
      latch = 1;
    } else {
  	//return in SetLatch();
    }
  }

}

void* waker5(void* arg)
{
  
  for(;;)
  {
    work_to_do = 1;

    //SetLatch();
    if(latch == 0)
    {
      latch = 1;
    } else {
  	//return in SetLatch();
    }
  }

}



void* waker6(void* arg)
{
  
  for(;;)
  {
    work_to_do = 1;

    //SetLatch();
    if(latch == 0)
    {
      latch = 1;
    } else {
  	//return in SetLatch();
    }
  }

}


void* waker7(void* arg)
{
  
  for(;;)
  {
    work_to_do = 1;

    //SetLatch();
    if(latch == 0)
    {
      latch = 1;
    } else {
  	//return in SetLatch();
    }
  }

}

void* waker8(void* arg)
{
  
  for(;;)
  {
    work_to_do = 1;

    //SetLatch();
    if(latch == 0)
    {
      latch = 1;
    } else {
  	//return in SetLatch();
    }
  }

}

void* waker9(void* arg)
{
  
  for(;;)
  {
    work_to_do = 1;

    //SetLatch();
    if(latch == 0)
    {
      latch = 1;
    } else {
  	//return in SetLatch();
    }
  }

}

void* waker10(void* arg)
{
  
  for(;;)
  {
    work_to_do = 1;

    //SetLatch();
    if(latch == 0)
    {
      latch = 1;
    } else {
  	//return in SetLatch();
    }
  }

}


int main()
{
__CPROVER_ASYNC_0: waiter1(0);
__CPROVER_ASYNC_1: waiter2(0);
__CPROVER_ASYNC_2: waker1(0);
__CPROVER_ASYNC_3: waker2(0);
__CPROVER_ASYNC_4: waker3(0);
__CPROVER_ASYNC_5: waker4(0);
__CPROVER_ASYNC_6: waker5(0);

__CPROVER_ASYNC_7: waker6(0);
__CPROVER_ASYNC_8: waker7(0);
__CPROVER_ASYNC_9: waker8(0);
__CPROVER_ASYNC_10: waker9(0);
__CPROVER_ASYNC_11: waker10(0);

  return 0;
}

