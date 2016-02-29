#include <multiX.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <utilities/logging.h>

double time_diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return (double)temp.tv_sec*1e9 +  (double)temp.tv_nsec;
}


typedef struct{
  double min;
  double max;
} minmax_t;

typedef struct{
  double *array;
  unsigned N;
  double *output;
}my_args_t;

minmax_t find_min_max(double *array, unsigned N){
  double max = -DBL_MAX;
  double min = -max;
  for(unsigned n = 0; n < N; n++){
    double item = array[n];
    if(item < min){
      min = item;
    }
    if(item > max){
      max = item;
    }
  }
  return (minmax_t){.max = max, .min = min};
}

void my_work_func(void* args){
  my_args_t *my_args = args;
  double *array = my_args->array;
  double *output = my_args->output;
  unsigned N = my_args->N;
  minmax_t min_max = find_min_max(array, N);
  output[0] = min_max.min;
  output[1] = min_max.max;
}


int main(int argc, char **argv){
  const size_t array_size = (1 << 23);
  double *array = malloc(sizeof(double)*array_size);
  for(unsigned u = 0; u < array_size; u++){
    array[u] = drand48();
  }
  double T;
  const unsigned n_loops = 15;
  struct timespec t0, t1;

  clock_gettime(CLOCK_MONOTONIC, &t0);
  minmax_t min_max;
  for(unsigned u = 0; u < n_loops; u++){
    min_max = find_min_max(array, array_size);
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  double baseline = time_diff(t0, t1);
  report(INFO,"#Found bounds %f / %f in %ens\n", min_max.min, min_max.max, baseline);

  workQ_t *Q = new_workQ(0);
  int n_work_max = array_size/8;
  my_args_t *args = malloc(sizeof(my_args_t)*n_work_max);
  double *output = malloc(sizeof(double)*n_work_max*2);
  for(int n_work = 1; n_work <= n_work_max; n_work*=2){
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for(unsigned u = 0; u < n_loops; u++){
      for(int i = 0; i < n_work; i++){
        args[i].array = &array[i*(array_size/n_work)];
        args[i].N = array_size/n_work;
        args[i].output = &output[2*i];
        add_work_item(Q, &my_work_func, &args[i]);
      }
      finish(Q);
      min_max = find_min_max(output, n_work*2);
    }
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double time_taken = time_diff(t0, t1);
    double speedup = baseline/time_taken;
    printf("%d %e %e\n", n_work, time_taken, speedup);
  }
  return 0;
}
