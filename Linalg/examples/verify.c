#include <bench.h>
#include <matrix.h>
#include <stdio.h>
#include <math.h>

#include <stdlib.h>
#include <mm_malloc.h>

#define N 512 //size of our matrix measured in doubles. Always a multiple of 4 to make our lives easier

int reinitialize_matrices(v4d **A, v4d **B, v4d **C, v4d **D){
  int ret = 0;
  puts("Initializing...");
  v4d* A_2 = _mm_malloc(N*N*sizeof(double),64);//Allign to cachelines instead.
  v4d* B_2 = _mm_malloc(N*N*sizeof(double),64);
  v4d* C_2 = _mm_malloc(N*N*sizeof(double),64);
  v4d* D_2 = _mm_malloc(N*N*sizeof(double),64);
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
  _mm_free(*D);
  *A = A_2;
  *B = B_2;
  *C = C_2;
  *D = D_2;
  //puts("wiping cache...");
  //wipe_cache();
  return ret;
}

double verify(double* A, double *B, unsigned n){
  double max_diff = 0;
  for(unsigned y = 0; y < n; y++)
    for(unsigned x = 0; x < n; x++){
      double diff = fabs(A[y*n+x]-B[y*n+x]);
      if(diff > max_diff)
        max_diff = diff;
    }
  return max_diff;
}

void print_matrix(double *A, unsigned n){
  for(unsigned y = 0; y < n; y++){
    for(unsigned x = 0; x < n; x++)
      printf("%1.2f ", A[y*n+x]);
    puts("");
  }
}


int main(int argc, char** argv){
  v4d *A = NULL, *B = NULL, *C = NULL, *Reference = NULL;
  reinitialize_matrices(&A, &B, &C, &Reference);
//  puts("A =========================");
//  print_matrix((double*)A, N);
//  puts("B =========================");
//  print_matrix((double*)B, N);
  #define LOOPS (1 << 0)
  puts("Computing reference...");
  MxM_square_scalar((double*)A,(double*)B,(double*)Reference, N);
  //puts("R =========================");
  //print_matrix((double*)Reference, N);
  puts("Reference computed!");
  puts("Verifying 1...");
  MxM_square(A,B,C, N);
  printf("Max diff = %e\n", verify((double*)C, (double*)Reference, N));
  //puts("C =========================");
  //print_matrix((double*)C, N);
  puts("Verifying 2...");
  MxM_square_2(A,B,C, N);
  printf("Max diff = %e\n", verify((double*)C, (double*)Reference, N));
  puts("Verifying 3...");
  MxM_square_3(A,B,C, N);
  printf("Max diff = %e\n", verify((double*)C, (double*)Reference, N));
  puts("Verifying T...");
  M_transpose(B, C, N);
  MxM_square_T(A,C, B, N);
  printf("Max diff = %e\n", verify((double*)B, (double*)Reference, N));
  //puts("C =========================");
  //print_matrix((double*)B, N);
  return 0;
}
