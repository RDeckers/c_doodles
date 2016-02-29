#include <linalg/matrix.h>
#include <stdio.h>

#include <stdlib.h>
#include <mm_malloc.h>

#include <time.h>

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

#define N 8*64 //size of our matrix measured in doubles. Always a multiple of 4 to make our lives easier
#define _aligned_malloc(S,A) aligned_malloc(A,S)

void wipe_cache() __attribute__((optimize(0)));

void wipe_cache(){
  size_t cache_size = 16*(1 << 20);
  size_t count = 0;
  char *tmp = malloc(cache_size);
  for(unsigned u = 0; u < cache_size; u++){
    tmp[u] = rand() & 0xFF;
  }
  for(unsigned u = 1; u < cache_size-1; u++){
    if(0 == tmp[u-1]*tmp[u+1]-tmp[u])
      count++;
  }
  printf("Count was %lu\n", count);
  free(tmp);
}

int reinitialize_matrices(v4d **A, v4d **B, v4d **C){
  int ret = 0;
  puts("Initializing...");
  v4d* A_2 = _mm_malloc(N*N*sizeof(double),64);//Allign to cachelines instead.
  v4d* B_2 = _mm_malloc(N*N*sizeof(double),64);
  v4d* C_2 = _mm_malloc(N*N*sizeof(double),64);
  ret = ((A_2==NULL)||(B_2==NULL)||(C_2==NULL));
  if(ret != 0)
    return ret;
  for(unsigned y = 0; y < N; y++){
    for(unsigned x = 0; x < N; x++){
      ((double*)A_2)[y*N+x] = rand() /((double) RAND_MAX);
      ((double*)B_2)[y*N+x] = rand() /((double) RAND_MAX);
      //((double*)C_2)[y*N+x] = rand() /((double) RAND_MAX);
    }
  }
  _mm_free(*A);
  _mm_free(*B);
  _mm_free(*C);
  *A = A_2;
  *B = B_2;
  *C = C_2;
  puts("wiping cache...");
  wipe_cache();
  return ret;
}


int main(int argc, char** argv){
  v4d *A = NULL, *B = NULL, *C = NULL;
  reinitialize_matrices(&A, &B, &C);
  #define LOOPS (1 << 0)
  struct timespec t0, t1;

  size_t T;
  puts("Running simd...");
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for(int i = 0; i < LOOPS; i++){
      MxM_square(A,B,C, N);
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  printf("vector_1\tticks/iteration: %e\n", time_diff(t0,t1));
  reinitialize_matrices(&A, &B, &C);
  puts("Running simd_2...");
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for(int i = 0; i < LOOPS; i++){
    MxM_square_2(A,B,C, N);
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  printf("vector_2\tticks/iteration: %e\n", time_diff(t0,t1));

  reinitialize_matrices(&A, &B, &C);
  puts("Running simd_3...");
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for(int i = 0; i < LOOPS; i++){
    MxM_square_3(A,B,C, N);
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  printf("vector_3\tticks/iteration: %e\n", time_diff(t0,t1));

  reinitialize_matrices(&A, &B, &C);
  puts("Running simd_T...");
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for(int i = 0; i < LOOPS; i++){
    MxM_square_T(A,B,C, N);
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  printf("vector_T\tticks/iteration: %e\n", time_diff(t0,t1));

  reinitialize_matrices(&A, &B, &C);
  puts("Running scalar...");
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for(int i = 0; i < LOOPS; i++){
    MxM_square_scalar((double*)A,(double*)B,(double*)C, N);
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  printf("scalar\tticks/iteration: %e\n", time_diff(t0,t1));
  return 0;
}
