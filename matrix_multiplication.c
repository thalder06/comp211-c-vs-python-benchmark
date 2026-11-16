#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// High-resolution monotonic timer in seconds
double get_time_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// Standard O(N^3) matrix multiplication: C = A * B
void matmul(const double* A, const double* B, double* C, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}

// Comparator for qsort to extract median execution time
int cmp_double(const void* a, const void* b) {
    double diff = (*(const double*)a - *(const double*)b);
    return (diff > 0) - (diff < 0);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <matrix_size_N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int runs = 5;

    // Allocate contiguous 1D blocks on the heap
    double* A = malloc((size_t)N * N * sizeof(double));
    double* B = malloc((size_t)N * N * sizeof(double));
    double* C = malloc((size_t)N * N * sizeof(double));

    if (!A || !B || !C) {
        fprintf(stderr, "Memory allocation failed\n");
        free(A);
        free(B);
        free(C);
        return 1;
    }

    // Deterministic initialization
    srand(42);
    for (int i = 0; i < N * N; i++) {
        A[i] = (double)rand() / RAND_MAX;
        B[i] = (double)rand() / RAND_MAX;
    }

    // 1. Warm-up run (untimed)
    matmul(A, B, C, N);

    // 2. Timed trials
    double times[5];
    for (int r = 0; r < runs; r++) {
        double start = get_time_sec();
        matmul(A, B, C, N);
        double end = get_time_sec();
        times[r] = end - start;
    }

    qsort(times, runs, sizeof(double), cmp_double);
    double median = times[runs / 2];

    // Print C[0] so GCC -O3 cannot optimize away the computation
    printf("N=%d | Median Time: %.6f s | Checksum C[0]: %.4f\n", N, median,
           C[0]);

    free(A);
    free(B);
    free(C);
    return 0;
}