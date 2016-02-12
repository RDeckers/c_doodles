#ifndef _MY_MATRIX_H
#define _MY_MATRIX_H

#include <simd_types.h>

typedef v4d matrix_4x4_t[4];

void MxM_square(v4d* A, v4d* B, v4d* C, unsigned N);
void MxM_square_T(v4d* A, v4d* B, v4d* C, unsigned N);
void M_transpose(v4d* A, v4d* At, unsigned N);
void MxM_square_2(v4d* A, v4d* B, v4d* C, unsigned N);
void MxM_square_3(v4d* A, v4d* B, v4d* C, unsigned N);
void MxM_square_scalar(double* A, double* B, double* C, unsigned N);

void load_transpose_4x4(v4d* A, v4d *At);
void print_v4d(v4d v);
void print_matrix_4x4(v4d* A);

void MxM_4x4(v4d *A, v4d *B, v4d *C);
void MxM_4x4_2(v4d *A, v4d *B, v4d *C);
double dot_vec(v4d *x, v4d* y, const int N);

#endif
