#include <utilities/benchmarking.h>

double time_diff(struct timespec *start, struct timespec *end)
{
    struct timespec tmp;
    if ((end->tv_nsec-start->tv_nsec)<0) {
        tmp.tv_sec = end->tv_sec-start->tv_sec-1;
        tmp.tv_nsec = 1000000000+end->tv_nsec-start->tv_nsec;
    } else {
        tmp.tv_sec = end->tv_sec-start->tv_sec;
        tmp.tv_nsec = end->tv_nsec-start->tv_nsec;
    }
    return (double)tmp.tv_sec*1e9 +  (double)tmp.tv_nsec;
}

double elapsed_since(struct timespec *T){
  struct timespec tmp;
  clock_gettime(CLOCK_MONOTONIC, &tmp);
  return time_diff(T, &tmp);
}

double tock(struct timespec *T){
  struct timespec tmp;
  tick(&tmp);
  double elapsed = time_diff(T, &tmp);
  *T = tmp;
  return elapsed;
}

void tick(struct timespec *T){
  clock_gettime(CLOCK_MONOTONIC, T);
}
