#include <bench.h>
#include <matrix.h>
#include <stdio.h>

#include <stdlib.h>
#include <intrin.h>
#include <malloc.h>

#define N 4*64 //size of our matrix measured in doubles. Always a multiple of 4 to make our lives easier

int main(int argc, char** argv){
  v4d *A, *B, *C_scalar, *C_simd;
  puts("Initializing...");
  A = _aligned_malloc(N*N*sizeof(double),32);
  B = _aligned_malloc(N*N*sizeof(double),32);
  C_scalar = _aligned_malloc(N*N*sizeof(double),32);
  C_simd = _aligned_malloc(N*N*sizeof(double),32);
  if((A==NULL)||(B==NULL)||(C_scalar==NULL)||(C_simd==NULL)){
    puts("allocation failed!");
    return -1;
  }
  for(unsigned y = 0; y < N; y++){
    for(unsigned x = 0; x < N; x++){
      ((double*)A)[y*N+x] = rand();
      ((double*)B)[y*N+x] = rand();
      ((double*)C_scalar)[y*N+x] = rand();
      ((double*)C_simd)[y*N+x] = rand();
    }
  }
  puts("Running scalar...");
  #define LOOPS (1 << 4)
  size_t T;
  TIME_CALL(
    for(int i = 0; i < LOOPS; i++){
      MxM_square_scalar((double*)A,(double*)B,(double*)C_scalar, N);
    },
    T
  );
  printf("scalar\tticks/iteration: %f\n", ((double)T)/LOOPS);
  puts("Running simd...");
  TIME_CALL(
    for(int i = 0; i < LOOPS; i++){
      MxM_square(A,B,C_simd, N);
    },
    T
  );
  printf("vector\tticks/iteration: %f\n", ((double)T)/LOOPS);
  return 0;
}
