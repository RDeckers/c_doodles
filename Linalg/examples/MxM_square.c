#include <bench.h>
#include <matrix.h>
#include <stdio.h>

#include <stdlib.h>
#include <intrin.h>
#include <malloc.h>

#define N 4*512 //size of our matrix measured in doubles. Always a multiple of 4 to make our lives easier


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
  v4d* A_2 = _aligned_malloc(N*N*sizeof(double),64);//Allign to cachelines instead.
  v4d* B_2 = _aligned_malloc(N*N*sizeof(double),64);
  v4d* C_2 = _aligned_malloc(N*N*sizeof(double),64);
  ret = ((A_2==NULL)||(B_2==NULL)||(C_2==NULL));
  if(ret != 0)
    return ret;
  for(unsigned y = 0; y < N; y++){
    for(unsigned x = 0; x < N; x++){
      ((double*)A_2)[y*N+x] = rand();
      ((double*)B_2)[y*N+x] = rand();
      ((double*)C_2)[y*N+x] = rand();
    }
  }
  _aligned_free(*A);
  _aligned_free(*B);
  _aligned_free(*C);
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
  size_t T;
  puts("Running simd...");
  TIME_CALL(
    for(int i = 0; i < LOOPS; i++){
      MxM_square(A,B,C, N);
    },
    T
  );
  printf("vector\tticks/iteration: %.6e\n", ((double)T)/LOOPS);
  reinitialize_matrices(&A, &B, &C);
  puts("Running simd_2...");
  TIME_CALL(
    for(int i = 0; i < LOOPS; i++){
      MxM_square_2(A,B,C, N);
    },
    T
  );
  printf("vector2\tticks/iteration: %.6e\n", ((double)T)/LOOPS);
  reinitialize_matrices(&A, &B, &C);
  puts("Running scalar...");
  TIME_CALL(
    for(int i = 0; i < LOOPS; i++){
      MxM_square_scalar((double*)A,(double*)B,(double*)C, N);
    },
    T
  );

  printf("scalar\tticks/iteration: %.6e\n", ((double)T)/LOOPS);
  return 0;
}
