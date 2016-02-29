#include <multiX.h>
#include <stdio.h>
#include <utilities/logging.h>

void my_work_func(work_item_dq_t* parent, void* args){//http://stackoverflow.com/a/13001749/5133184
  int *x = args;
  long n = *x;
  int count=0;
  long a = 2;
  while(count<n)
    {
        long b = 2;
        int prime = 1;// to check if found a prime
        while(b * b <= a)
        {
            if(a % b == 0)
            {
                prime = 0;
                break;
            }
            b++;
        }
        if(prime > 0)
        count++;
        a++;
    }
    *x = (--a);
}

int main(int argc, char **argv){
  workQ_t *Q = new_workQ(0);
  report(PASS, "Created a work Q");
  const int n_work = 1024;
  long data[n_work];
  for(int i = 0; i < n_work; i++){
    data[i] = i+1;
    add_work_item(Q, &my_work_func, &data[i]);
  }
  finish(Q);
  for(int i = 1; i < n_work; i++){
    printf("%d]\t%d\n", i, data[i]);
  }
  return 0;
}
