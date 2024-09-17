#pragma once

#include <emmintrin.h>
#include <immintrin.h>

#define A(i, j) A[(i) + (j)*LDA]
#define B(i, j) B[(i) + (j)*LDB]
#define C(i, j) C[(i) + (j)*LDC]

#define STEP 8

void scale_c(double *C, int M, int N, int LDC, double scalar) {
  int i, j;
#pragma omp parallel for collapse(2) private(j)
  for (i = 0; i < M; i++) {
    for (j = 0; j < N; j++) {
      C(i, j) *= scalar;
    }
  }
}

void mydgemm_cpu_ord(int M, int N, int K, double alpha, double *A, int LDA,
                     double *B, int LDB, double beta, double *C, int LDC) {
  int i, j, k;
  if (beta != 1.0)
    scale_c(C, M, N, LDC, beta);
#pragma omp parallel for collapse(3) private(j, k)
  for (i = 0; i < M; i++) {
    for (j = 0; j < N; j++) {
      for (k = 0; k < K; k++) {
        C(i, j) += alpha * A(i, k) * B(k, j);
      }
    }
  }
}

void mydgemm_cpu_v1(int M, int N, int K, double alpha, double *A, int LDA,
                    double *B, int LDB, double beta, double *C, int LDC) {
  int i, j, k;
  if (beta != 1.0)
    scale_c(C, M, N, LDC, beta);
  for (i = 0; i < M; i++) {
    for (j = 0; j < N; j++) {
      for (k = 0; k < K; k++) {
        C(i, j) += alpha * A(i, k) * B(k, j);
      }
    }
  }
}

void mydgemm_cpu_kern(int M, int N, int K, double alpha, double *A, int LDA,
                      double *B, int LDB, double *C, int LDC) {
  int i, j, k;
  int Mr = M % STEP, Nr = N % STEP;
  __m512d valpha = _mm512_set1_pd(alpha);
#pragma omp parallel for collapse(2) private(j, k)
  for (i = 0; i < M - Mr; i += STEP) {
    for (j = 0; j < N - Nr; j += STEP) {
      __m512d c0 = _mm512_setzero_pd(), c1 = _mm512_setzero_pd(),
              c2 = _mm512_setzero_pd(), c3 = _mm512_setzero_pd(),
              c4 = _mm512_setzero_pd(), c5 = _mm512_setzero_pd(),
              c6 = _mm512_setzero_pd(), c7 = _mm512_setzero_pd();
      for (k = 0; k < K; k++) {
        __m512d a = _mm512_mul_pd(valpha, _mm512_loadu_pd(&A(i, k)));
        __m512d b0 = _mm512_broadcastsd_pd(_mm_set_pd1(B(k, j))),
                b1 = _mm512_broadcastsd_pd(_mm_set_pd1(B(k, j + 1))),
                b2 = _mm512_broadcastsd_pd(_mm_set_pd1(B(k, j + 2))),
                b3 = _mm512_broadcastsd_pd(_mm_set_pd1(B(k, j + 3))),
                b4 = _mm512_broadcastsd_pd(_mm_set_pd1(B(k, j + 4))),
                b5 = _mm512_broadcastsd_pd(_mm_set_pd1(B(k, j + 5))),
                b6 = _mm512_broadcastsd_pd(_mm_set_pd1(B(k, j + 6))),
                b7 = _mm512_broadcastsd_pd(_mm_set_pd1(B(k, j + 7)));
        c0 = _mm512_fmadd_pd(a, b0, c0);
        c1 = _mm512_fmadd_pd(a, b1, c1);
        c2 = _mm512_fmadd_pd(a, b2, c2);
        c3 = _mm512_fmadd_pd(a, b3, c3);
        c4 = _mm512_fmadd_pd(a, b4, c4);
        c5 = _mm512_fmadd_pd(a, b5, c5);
        c6 = _mm512_fmadd_pd(a, b6, c6);
        c7 = _mm512_fmadd_pd(a, b7, c7);
      }
      _mm512_storeu_pd(&C(i, j), _mm512_add_pd(c0, _mm512_loadu_pd(&C(i, j))));
      _mm512_storeu_pd(&C(i, j + 1),
                       _mm512_add_pd(c1, _mm512_loadu_pd(&C(i, j + 1))));
      _mm512_storeu_pd(&C(i, j + 2),
                       _mm512_add_pd(c2, _mm512_loadu_pd(&C(i, j + 2))));
      _mm512_storeu_pd(&C(i, j + 3),
                       _mm512_add_pd(c3, _mm512_loadu_pd(&C(i, j + 3))));
      _mm512_storeu_pd(&C(i, j + 4),
                       _mm512_add_pd(c4, _mm512_loadu_pd(&C(i, j + 4))));
      _mm512_storeu_pd(&C(i, j + 5),
                       _mm512_add_pd(c5, _mm512_loadu_pd(&C(i, j + 5))));
      _mm512_storeu_pd(&C(i, j + 6),
                       _mm512_add_pd(c6, _mm512_loadu_pd(&C(i, j + 6))));
      _mm512_storeu_pd(&C(i, j + 7),
                       _mm512_add_pd(c7, _mm512_loadu_pd(&C(i, j + 7))));
    }
  }
  if (!Mr && !Nr)
    return;
  // boundary not STEPxSTEP aligned
  if (Mr)
    mydgemm_cpu_ord(Mr, N, K, alpha, A + M - Mr, LDA, B, LDB, 1.0,
                    &C(M - Mr, 0), LDC);
  if (Nr)
    mydgemm_cpu_ord(M - Mr, Nr, K, alpha, A, LDA, &B(0, N - Nr), LDB, 1.0,
                    &C(0, N - Nr), LDC);
}

void mydgemm_cpu(int M, int N, int K, double alpha, double *A, int LDA,
                 double *B, int LDB, double beta, double *C, int LDC) {

  if (beta != 1.0)
    scale_c(C, M, N, LDC, beta);

  mydgemm_cpu_kern(M, N, K, alpha, A, LDA, B, LDB, C, LDC);
}
