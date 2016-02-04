#include <matrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>

void load_transpose_4x4(v4d* A, v4d *At){
  for(int i = 0; i < 4; i++){
    At[0][i] = A[i][0];
    At[1][i] = A[i][1];
    At[2][i] = A[i][2];
    At[3][i] = A[i][3];
  }
}

void print_v4d(v4d v){
  printf("[%1.2f %1.2f %1.2f %1.2f]\n",
        v[0], v[1], v[2], v[3]);
}

void print_matrix_4x4(v4d* A){
  for(int i = 0; i < 4; i++){
    print_v4d(A[i]);
  }
}

void MxM_square_scalar(double* A, double* B, double* C, unsigned N){
  for (int j = 0; j < N; j++) {
    for (int i = 0; i < N; i++) {
      C[i+N*j] = 0.0;
      for (int k = 0; k < N; k++) {
        C[i+N*j] += A[i+N*k] * B[k+N*j];
      }
    }
  }
}

void MxM_square(v4d* A, v4d* B, v4d* C, unsigned N){
  const unsigned N_reduced = N/4;
  for(unsigned yA = 0; yA < N; yA++){
    for(unsigned xB = 0; xB < N_reduced; xB++){
      v4d sum = {0,0,0,0};
      for(unsigned xA = 0; xA < N; xA++){
        v4d broadcast = _mm256_broadcast_sd(((double*)(&(A[yA*N_reduced])))+xA);//for very large matrices, this CL could get evicted before it's used again?
        //buffer 8 broadcasts (1 CL) in registers so that we have at most a 1/8 miss.
        sum += broadcast*B[N_reduced*xA+xB];
      }
      C[xB+N_reduced*xB] = sum;
    }
  }
}

void MxM_square_2(v4d* A, v4d* B, v4d* C, unsigned N){
  const unsigned N_reduced = N/4;
  for(unsigned yA = 0; yA < N; yA++){
    for(unsigned xB = 0; xB < N_reduced; xB+=2){
      v4d sum[2] = {{0,0,0,0}, {0,0,0,0}};
      for(unsigned xA = 0; xA < N; xA++){
        v4d broadcast = _mm256_broadcast_sd(((double*)(&(A[yA*N_reduced])))+xA);
        sum[0] += broadcast*B[N_reduced*xA+xB];
        sum[1] += broadcast*B[N_reduced*xA+xB+1];
      }
      C[xB+N_reduced*xB] = sum[0];
      C[xB+N_reduced*xB+1] = sum[1];
    }
  }
}

//TODO: transpose method like below and/or in place transpose of B
//TODO: cached broadcasts (cache into single register, then broadcast from register?)
//TODO: Run Intel thingy to measure actual cache hit ratio.
//TODO: Pin to core/high process affinity for more accurate measurements under load.
//TODO: Pinned multithreading.

void MxM_4x4(v4d *A, v4d *B, v4d *C){
  matrix_4x4_t B_t;
  load_transpose_4x4(B, B_t);

  for(int i = 0; i < 4; i++){
    v4d tmp_col_1 = A[i] * B_t[0];
    v4d tmp_col_2 = A[i] * B_t[1];
    v4d tmp_col_3 = A[i] * B_t[2];
    v4d tmp_col_4 = A[i] * B_t[3];

    v4d x = __builtin_ia32_haddpd256(tmp_col_1, tmp_col_2); //(1,1)+(1,2), (2,1)+(2,2)| (1,3)+(1,4), (2,3)+(2,4)
    v4d y = __builtin_ia32_haddpd256(tmp_col_3, tmp_col_4); //(3,1)+(3,2), (4,1)+(4,2)| (3,3)+(3,4), (4,3)+(4,4)

    v4d first = __builtin_ia32_vperm2f128_pd256(y,x, 0b000010); // 1(1,2), 2(1,2) | 3(1,2), 4(1,2)
    v4d last = __builtin_ia32_vperm2f128_pd256(y,x, 0b010011); // 1(3,4), 2(3,4) | 3(3,4),4(3,4)
    C[i] = first+last;
  }
}

void MxM_4x4_2(v4d* A, v4d* B, v4d *C){
//Intel's method (https://software.intel.com/en-us/articles/benefits-of-intel-avx-for-small-matrices)
//After compiler optimizations.
//Faster then O2 of version one, but slower than the O3 version.
  for(int row = 0; row < 4; row++){
    v4d broadcast[4];
    for(int i = 0; i < 4; i++)
      broadcast[i] = _mm256_broadcast_sd(((double*)&(A[row]))+i);
    v4d new_row = broadcast[0]*B[0];
    new_row += broadcast[1]*B[1];
    new_row += broadcast[2]*B[2];
    new_row += broadcast[3]*B[3];
    C[row] = new_row;
  }
}

double dot_vec(v4d *x, v4d* y, const int N){
  v4d accumulate = {0,0,0,0};
  int i;
  for (i = 0; i < N/4;i ++){
    accumulate += x[i]*y[i];
  }
  i *= 4;
  double sum = accumulate[0]+accumulate[1]+accumulate[2]+accumulate[3];
  for(;i < N; i++){
    sum += ((double *)x)[i]*((double*)y)[i];
  }
  return sum;
}

double dot_serial(double *x, double *y, const int N){
  double sum = 0;
  for (int i = 0; i < N;i++)
    sum += x[i]*y[i];
  return sum;
}
