volatile int c1 = 0, c2 = 0, c3 = 0, c4=0, n1= 0, n2 = 0, n3 = 0, n4=0;

void * thr1(void * arg)
{
  int max;
  
  while (1) {

	c1 = 1;
	if (c2 < c3) {
	  if (c4 < c3)
	  	max = c3;
	  else
		max = c4;
        } else {
	  if (c4 < c2)
	  	max = c2;
	  else 
		max = c4;
        }
	n1 = max + 1;
	c1 = 0;
       
	while (c2 != 0) {;}
	while (n2 > 0 && n2 < n1) {;} 
	
	while (c3 != 0) {;}
	while (n3 > 0 && n3 < n1) {;} 
	
	while (c4 != 0) {;}
	while (n4 > 0 && n4 < n1) {;} 
	
	// CS
	n1 = 0;
  }
}

void * thr2(void * arg)
{
  int max;
  
  while (1) {

	c2 = 1;
	if (c1 < c3) {
	  if (c4 < c3)
	  	max = c3;
	  else 
		max = c4;
        } else {
	  if (c4 < c1)
	  	max = c1;
	  else 
		max = c4;
        }
	n2 = max + 1;
	c2 = 0;
       
	while (c1 != 0) {;}
	while (n1 > 0 && n1 <= n2) {;} 
	
	while (c3 != 0) {;}
	while (n3 > 0 && n3 < n2) {;} 
	
	while (c4 != 0) {;}
	while (n4 > 0 && n4 < n2) {;}

	//CS
	n2 = 0;
  }
}


void * thr3(void * arg)
{
  int max;
 
  while (1) {

	c3 = 1;
	if (c1 < c2)  {
 	  if (c4 < c2)
	  	max = c2;
	  else 
		max = c4;
        } else {
  	   if (c4 < c1)
	  	max = c1;
	   else 
		max = c4;
        }
	n3 = max + 1;
	c3 = 0;
       
	while (c1 != 0) {;}
	while (n1 > 0 && n1 <= n3) {;} 
	
	while (c2 != 0) {;}
	while (n2 > 0 && n2 <= n3) {;} 
	
	while (c4 != 0) {;}
	while (n4 > 0 && n4 < n3) {;}

	n3 = 0;
  }
}

void * thr4(void * arg)
{
  int max;
 
  while (1) {

	c4 = 1;
	if (c1 < c2)  {
 	  if (c3 < c2)
	  	max = c2;
	  else 
		max = c3;
        } else {
  	   if (c3 < c1)
	  	max = c1;
	   else 
		max = c3;
        }
	n4 = max + 1;
	c4 = 0;
       
	while (c1 != 0) {;}
	while (n1 > 0 && n1 <= n4) {;} 
	
	while (c2 != 0) {;}
	while (n2 > 0 && n2 <= n4) {;} 
	
	while (c3 != 0) {;}
	while (n3 > 0 && n3 <= n4) {;}

	n4 = 0;
  }
}

int main()
{
  __CPROVER_ASYNC_0: thr1(0);
  __CPROVER_ASYNC_1: thr2(0);
  __CPROVER_ASYNC_2: thr3(0);
  __CPROVER_ASYNC_3: thr4(0);
return 0;
}

