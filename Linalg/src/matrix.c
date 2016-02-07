#include <matrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>

inline void load_transpose_4x4(v4d* A, v4d *At){
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
  for (int yA = 0; yA < N; yA++) {
    for (int xB = 0; xB < N; xB++) {
      double tmp = 0.0;
      for (int loop = 0; loop < N; loop++) {
        tmp += A[yA*N+loop] * B[xB+N*loop];
      }
      C[yA*N+xB] = tmp;
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
      C[xB+N_reduced*yA] = sum;
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
      C[xB+N_reduced*yA] = sum[0];
      C[xB+N_reduced*yA+1] = sum[1];
    }
  }
}
//
// void MxM_square_4(v4d* A, v4d* B, v4d* C, unsigned N){
//   const unsigned N_reduced = N/4;
//   for(unsigned yA = 0; yA < N; yA++){
//     for(unsigned xB = 0; xB < N_reduced; xB+=2){
//       v4d sum[2] = {{0,0,0,0}, {0,0,0,0}};
//       for(unsigned xA = 0; xA < N; xA++){
//         v4d broadcast = _mm256_broadcast_sd(((double*)(&(A[yA*N_reduced])))+xA);
//         const unsigned base = xB + N_reduced*Xa;
//         const unsigned stride = 1;
//         sum[0] += broadcast*B[base+0*stride];
//         sum[1] += broadcast*B[base+1*stride];
//       }
//       C[xB+N_reduced*xB] = sum[0];
//       C[xB+N_reduced*xB+1] = sum[1];
//     }
//   }
// }

//TODO: transpose method like below and/or in place transpose of B
//TODO: cached broadcasts (cache into single register, then broadcast from register?)
//TODO: Run Intel thingy to measure actual cache hit ratio.
//TODO: Pin to core/high process affinity for more accurate measurements under load.
//TODO: Pinned multithreading.
//TODO: turn off hardware prefetcher and time again.
//TODO: use blocking from "more SW optimization"

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
//
// void MxM_square_3(v4d* A, v4d* B, v4d* C, unsigned N){
//   v4d ymm0, ymm1, ymm2, ymm3, ymm4, ymm5, ymm6, ymm7, ymm8, ymm9, ymm10, ymm11, ymm12, ymm13, ymm14, ymm15;
//   const unsigned N_reduced = N/4;
//   for(unsigned yA = 0; yA < N; yA+=4){
//     for(unsigned xB = 0; xB < N_reduced; xB+=2){//use 2 xB per loop to maximize cache hits
//       //output matrices here as well, 8 registers max.
//       for(unsigned dot_step = 0; dot_step < N_reduced; dot_step++){
//         const unsigned stride = N_reduced;
//         const unsigned base_A = yA*N_reduced+dot_step;
//         //v4d A_sub[4] = {A[base_A+0*stride], A[base_A+1*stride], A[base_A+2*stride], A[base_A+3*stride]}; //4 registers
//         ymm0 = A[base_A+0*stride];
//         ymm1 = A[base_A+1*stride];
//         ymm2 = A[base_A+2*stride];
//         //Now we need to load a block of B and transpose it. with AVX2 we could use VGATHERPD but my FX-8120 doesn't support that
//         ymm3 = A[base_A+3*stride];
//         const unsigned base_B = xB+N_reduced*4*dot_step;
//         //v4d B_sub[4] = {B[base_B+0*stride], B[base_B+1*stride], B[base_B+2*stride], B[base_B+3*stride]}; //8 registers
//         //v4d B_subT[4]; //12 registers + 4 registers for output
//         //load 2 B_sub, 8 registers total
//         ymm4 = B[base_B+0*stride];
//         ymm5 = B[base_B+1*stride];
//         ymm6 = _mm256_unpacklo_pd(ymm4, ymm5);
//         ymm7 = _mm256_unpackhi_pd(ymm4, ymm5);
//         //B_sub0,1 now free, load 2 and 3
//         //introduces dependency.
//         ymm4 = B[base_B+2*stride];
//         ymm5 = B[base_B+3*stride];
//         ymm8 = _mm256_unpacklo_pd(ymm4, ymm5);
//         ymm9 = _mm256_unpackhi_pd(ymm4, ymm5);
//         //10 registers max
//         //dependencies
//         ymm4 = _mm256_permute2f128_pd(ymm6, ymm8, 0) //TODO: get correct code for this, see above
//         ymm5 = _mm256_permute2f128_pd(ymm6, ymm8, 0) //TODO: get correct code for this
//         //done with 0 an 2,
//
//         ymm6 = _mm256_permute2f128_pd(ymm7, ymm9, 0) //TODO: get correct code for this
//         ymm7 = _mm256_permute2f128_pd(ymm7, ymm9, 0) //TODO: get correct code for this
//         //done with 1 and 3
//
//         //subT freed, 8 registers usable.
//         //B_sub now contains the transpose of B_sub.
//
//
//     }
//     }
//   }
// }

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
