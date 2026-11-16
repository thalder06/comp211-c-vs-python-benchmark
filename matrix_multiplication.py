import random
import statistics
import sys
import time

try:
    import numpy as np
    HAS_NUMPY = True
except ImportError:
    HAS_NUMPY = False


def matmul_python(A, B, N):
    C = [[0.0] * N for _ in range(N)]
    for i in range(N):
        for j in range(N):
            total = 0.0
            for k in range(N):
                total += A[i][k] * B[k][j]
            C[i][j] = total
    return C


def main():
    if len(sys.argv) < 2:
        print(f"Usage: python3 {sys.argv[0]} <matrix_size_N>")
        sys.exit(1)

    N = int(sys.argv[1])
    runs = 5

    random.seed(42)
    A = [[random.random() for _ in range(N)] for _ in range(N)]
    B = [[random.random() for _ in range(N)] for _ in range(N)]

    # --- 1. Pure Python Benchmark ---
    matmul_python(A, B, N)  # Warm-up run
    py_times = []
    C = None
    for _ in range(runs):
        start = time.perf_counter()
        C = matmul_python(A, B, N)
        end = time.perf_counter()
        py_times.append(end - start)

    py_median = statistics.median(py_times)
    print(f"[Pure Python] N={N} | Median Time: {py_median:.6f} s | Checksum C[0][0]: {C[0][0]:.4f}")

    # --- 2. NumPy Benchmark (if installed) ---
    if HAS_NUMPY:
        A_np = np.array(A, dtype=np.float64)
        B_np = np.array(B, dtype=np.float64)

        _ = A_np @ B_np  # Warm-up run
        np_times = []
        C_np = None
        for _ in range(runs):
            start = time.perf_counter()
            C_np = A_np @ B_np
            end = time.perf_counter()
            np_times.append(end - start)

        np_median = statistics.median(np_times)
        print(f"[NumPy (C)]   N={N} | Median Time: {np_median:.6f} s | Checksum C[0][0]: {C_np[0, 0]:.4f}")


if __name__ == "__main__":
    main()