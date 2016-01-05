#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <immintrin.h>
#if defined(__i386__)
static __inline__ uint32_t rdtsc(void)
{
    uint32_t x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}

#elif defined(__x86_64__)

static __inline__ uint64_t rdtsc(void)
{
    uint32_t hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (uint64_t)lo)|( ((uint64_t)hi)<<32 );
}

#endif


#define timeCall(x,t) t =rdtsc();\
x;\
t = rdtsc()-t


typedef double v4d __attribute__ ((vector_size (32)));

typedef struct{
  size_t n,m;
  double **rows;
}matrix_t;

typedef v4d matrix_4x4_t[4];

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


//
// matrix_t* alloc_matrix(size_t n, size_t m){
//   matrix_t *A = malloc(sizeof(matrix_t));
//   A->row = malloc(sizeof(double*)*m);
//   for(size_t i = 0; i < m; i++){
//     posix_memalign(A->row[i], 32, sizeof(double)*n);
//   }
//   A->m = m;
//   A->n = n;
//   return A;
// }
//
// void matrix_matrix_mul(matrix_t *A, matrix_t *B, matrix_t* C){
//
//   }
//   /*load 8x8 block from B = all 16 registers
//     swap 1/2 with 2/1 and 1/3 with 3/1
//     load first row
//       swap with
//
//       [(x x x) {x x x}] i'th-> i*rowlength
//        (x x x  {x x x
//        (x x x  {x x x
//     */
//
// }



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

int main(int argc, char** argv){
  matrix_4x4_t A, B, C;
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      A[i][j] = (i+1.0)/((j+1));
      B[i][j] = (j+i)/3.0;
    }
    printf("[%1.2f %1.2f %1.2f %1.2f] [%1.2f %1.2f %1.2f %1.2f]\n",
            A[i][0], A[i][1], A[i][2], A[i][3], B[i][0], B[i][1], B[i][2], B[i][3]);
  }
  #define LOOPS (1 << 20)
  size_t T;
  timeCall(
    for(int i = 0; i < LOOPS; i++){
      MxM_4x4(A,B,C);
    },
    T);
  printf("ticks: %lu\n", T);
  print_matrix_4x4(C);
  timeCall(
    for(int i = 0; i < LOOPS; i++){
      MxM_4x4_2(A,B,C);
    }
    ,T);
  printf("ticks: %lu\n", T);
  print_matrix_4x4(C);
  // double *x, *y;
  // const int N = 103;
  // const int M = 50000;
  // posix_memalign(&x, 32, sizeof(double)*N);
  // posix_memalign(&y, 32, sizeof(double)*N);
  // for(int i = 0; i < N; i++){
  //   x[i] = i+1;
  //   y[i] = 1;
  // }
  // size_t T;
  // double sum;
  // sum = 0;
  // timeCall(
  // {
  //   for(int i = 0; i < M; i++){
  //     sum += dot_vec(x,y,N);
  //   }
  // }
  // , T);
  // printf("vectorized:\t%f\t%lu\n", sum, T);
  //
  // sum = 0;
  // timeCall(
  // {
  //   for(int i = 0; i < M; i++){
  //     sum += dot_serial(x,y,N);
  //   }
  // }
  // , T);
  // printf("Serial:\t\t%f\t%lu\n", sum, T);
  return 0;
}
