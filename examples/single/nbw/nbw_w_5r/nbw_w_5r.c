volatile int ccf = 0, buf = 0;
 
void* writer(void * arg) {
   int ccf_writer;
L0:   ccf_writer = ccf;
   ccf = ccf_writer + 1;
   buf = 1;
   ccf = ccf_writer + 2;
   goto L0;
}

void* reader1(void * arg) {
   int ccf_begin, ccf_end, value;
  
L0:   
   ccf_begin = ccf;
   value = buf;
   ccf_end = ccf;

   if (ccf_end != ccf_begin || ccf_begin == 1) 
     goto L0;
} 

void* reader2(void * arg) {
   int ccf_begin, ccf_end, value;
  
L0:   
   ccf_begin = ccf;
   value = buf;
   ccf_end = ccf;

   if (ccf_end != ccf_begin || ccf_begin == 1) 
     goto L0;
}

void* reader3(void * arg) {
   int ccf_begin, ccf_end, value;
  
L0:   
   ccf_begin = ccf;
   value = buf;
   ccf_end = ccf;

   if (ccf_end != ccf_begin || ccf_begin == 1) 
     goto L0;
}     

void* reader4(void * arg) {
   int ccf_begin, ccf_end, value;
  
L0:   
   ccf_begin = ccf;
   value = buf;
   ccf_end = ccf;

   if (ccf_end != ccf_begin || ccf_begin == 1) 
     goto L0;
} 

void* reader5(void * arg) {
   int ccf_begin, ccf_end, value;
  
L0:   
   ccf_begin = ccf;
   value = buf;
   ccf_end = ccf;

   if (ccf_end != ccf_begin || ccf_begin == 1) 
     goto L0;
} 

int main()
{
  __CPROVER_ASYNC_1: writer(0);
  __CPROVER_ASYNC_2: reader1(0);
  __CPROVER_ASYNC_3: reader2(0);
  __CPROVER_ASYNC_4: reader3(0);
  __CPROVER_ASYNC_5: reader4(0);
  __CPROVER_ASYNC_6: reader5(0);




return 0;
}
