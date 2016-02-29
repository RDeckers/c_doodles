#include <linalg/matrix.h>
#include <stdio.h>

#include <stdlib.h>
#include <mm_malloc.h>

#define N 16 //size of our matrix measured in doubles. Always a multiple of 4 to make our lives easier
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

void print_matrix(double *A, unsigned n){
  for(int y = 0; y < n; y++){
    for(int x = 0; x < n; x++){
     printf("%2.0f ", A[y*n+x]);
    }
    puts("");
 }
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
      ((double*)A_2)[y*N+x] = x+y*N;
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
  print_matrix((double*)A, N);
  M_transpose(A, B, N);
  puts("==========");
  print_matrix((double*)B, N);
  return 0;
}
