# Performance Benchmarking: C vs. Python (Matrix Multiplication)

**COMP 211 Checkoff 3 — Self-Directed Exploration**

## Introduction
While it is widely known that compiled languages like C execute faster than interpreted languages like Python, measuring *how much* faster—and understanding *why*—requires careful benchmarking design. For this exploration, I investigated how to construct fair, reproducible performance benchmarks and measured the execution speed of $O(N^3)$ dense matrix multiplication across **Pure Python**, **unoptimized C (`-O0`)**, and **optimized C (`-O3`)**.

## What I Explored
To compare C and Python without skewing the results, I implemented the exact same triple-nested `i-j-k` matrix multiplication algorithm in both languages and tested it in the `learncli` Linux environment across scaling input sizes ($N = 100, 250, 400, 500$, plus a 1-billion-operation stress test at $N = 1000$ for C `-O3`).

My benchmarking methodology addressed four common measurement pitfalls:
1. **Isolating Algorithm Runtime from Startup Overhead:** Instead of using the terminal `time` command (which includes booting the Python interpreter and allocating memory), I used internal monotonic high-resolution timers (`clock_gettime(CLOCK_MONOTONIC)` in C and `time.perf_counter()` in Python) directly around the multiplication loops.
2. **Filtering OS Noise and Cold-Cache Effects:** Each test performs 1 untimed warm-up pass followed by 5 timed runs, reporting the **median** runtime to prevent background container scheduling spikes from distorting the data.
3. **Preventing Dead-Code Elimination:** Because GCC at `-O3` can detect and delete loops whose output is never used, both programs print a checksum (`C[0]`) after stopping the timer to force full computation.
4. **Testing Compiler Optimization Levels:** Comparing both `gcc -O0` (no optimization) and `gcc -O3` (aggressive loop/vector optimization) separated the speed gained from C's memory model from the speed gained by modern compiler transformations.

## Benchmark Results (`learncli` Linux Environment)

| Matrix Size ($N$) | Total Operations ($N^3$) | C `-O0` Median (s) | C `-O3` Median (s) | Pure Python Median (s) | Speedup (Python / C `-O0`) | Speedup (Python / C `-O3`) |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **100** | 1,000,000 | 0.002054 | 0.001218 | 0.025155 | 12.2x | 20.7x |
| **250** | 15,625,000 | 0.035186 | 0.008774 | 0.440482 | 12.5x | 50.2x |
| **400** | 64,000,000 | 0.128128 | 0.053290 | 2.618341 | 20.4x | 49.1x |
| **500** | 125,000,000 | 0.311049 | 0.071131 | 8.686769 | 27.9x | 122.1x |
| **1000** | 1,000,000,000 | — | 0.755399 | ~69.5 s *(projected)* | — | ~92x *(projected)* |

## Most Interesting Discoveries
- **The Performance Gap Widens as Data Outgrows CPU Cache:** At $N = 100$, optimized C (`-O3`) is **20.7x faster** than Pure Python (`1.2 ms` vs. `25.2 ms`). By $N = 500$, that gap expands to **122.1x faster** (`71.1 ms` vs. `8.69 s`). In C, allocating matrices as contiguous 1D blocks of 8-byte `double`s allows CPU cache lines (64 bytes) to load 8 numbers at once. In Python, a 2D list is an array of pointers scattered across the heap pointing to bulky `PyObject` structs (~28 bytes per float), leading to frequent pointer chasing, dynamic type checks, and cache misses.
- **Compiler Flags Provide a Massive Boost Within C Alone:** Enabling `-O3` reduced C execution time at $N = 500$ from `0.311 s` down to `0.071 s`—a **4.37x speedup** over unoptimized C (`-O0`) without changing a single line of source code, thanks to register allocation, loop unrolling, and SIMD vectorization.
- **1 Billion Operations in Under a Second:** At $N = 1000$ ($10^9$ multiply-accumulate operations), C (`-O3`) completed in **0.755 seconds**. Because $O(N^3)$ work increases by $2^3 = 8\times$ when doubling $N$ from 500 to 1000, Pure Python was projected to take ~69.5 seconds per run (over 7 minutes for a full trial set), demonstrating why computationally heavy Python libraries like NumPy delegate array math to compiled C/Fortran backends.

## Repository Structure
- `matrix_multiplication.c`: C implementation using contiguous 1D heap-allocated `double` arrays and `clock_gettime(CLOCK_MONOTONIC)`.
- `matrix_multiplication.py`: Pure Python implementation using nested lists and `time.perf_counter()` (with optional NumPy support).
- `run_benchmarks.sh`: Shell script to compile both C optimization levels (`-O0` and `-O3`) and execute the full benchmark suite.
- `slides.pdf`: Presentation slides created for the COMP 211 Checkoff 3 peer presentation.

## How to Run
Run the automated benchmark script:
```bash
chmod +x run_benchmarks.sh
./run_benchmarks.sh