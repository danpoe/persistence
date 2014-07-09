volatile int level[4];
volatile int waiting[3];
int N = 4;

void * thr0(void * arg)
{
int l;
int i = 0;

for (l=0; l < 3; l++) {
  level[i] = l;
  waiting[l] = i;
   while (waiting[l] == i && (level[1] >=l || level[2] >= l || level[3] >= l)) {;}
}
  level[i] = 0;
}

void * thr1(void * arg)
{
int l;
int i = 1;

for (l=0; l < 3; l++) {
  level[i] = l;
  waiting[l] = i;
   while (waiting[l] == i && (level[0] >=l || level[2] >= l || level[3] >= l)) {;}
}
  level[i] = 0;
}

void * thr2(void * arg)
{
int l;
int i = 2;

for (l=0; l < 3; l++) {
  level[i] = l;
  waiting[l] = i;
   while (waiting[l] == i && (level[0] >=l || level[1] >= l || level[3] >= l)) {;}
}
  level[i] = 0;
}

void * thr3(void * arg)
{
int l;
int i = 3;

for (l=0; l < 3; l++) {
  level[i] = l;
  waiting[l] = i;
   while (waiting[l] == i && (level[0] >=l || level[1] >= l || level[2] >= l)) {;}
}
  level[i] = 0;
}


int main()
{
  __CPROVER_ASYNC_0: thr0(0);
  __CPROVER_ASYNC_1: thr1(0);
  __CPROVER_ASYNC_2: thr2(0);
  __CPROVER_ASYNC_3: thr3(0);
return 0;
}


