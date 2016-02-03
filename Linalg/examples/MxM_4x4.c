#include <bench.h>
#include <matrix.h>
#include <stdio.h>

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
  #define LOOPS (1 << 10)
  size_t T;
  TIME_CALL(
    for(int i = 0; i < LOOPS; i++){
      MxM_4x4(A,B,C);
    },
    T
  );
  printf("ticks/iteration: %f\n", ((double)T)/LOOPS);
  print_matrix_4x4(C);
  TIME_CALL(
    for(int i = 0; i < LOOPS; i++){
      MxM_4x4_2(A,B,C);
    },
    T
  );
  printf("ticks/iteration: %f\n", ((double)T)/LOOPS);
  print_matrix_4x4(C);
  return 0;
}
