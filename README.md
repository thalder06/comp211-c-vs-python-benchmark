# Performance Benchmarking: C vs. Python (Matrix Multiplication)

**COMP 211 Checkoff 3 — Self-Directed Exploration**

## Overview
This repository explores how to design reliable performance benchmarks and compares the execution speed of $O(N^3)$ dense matrix multiplication across **Pure Python**, **unoptimized C (`-O0`)**, and **optimized C (`-O3`)** in a Linux container environment.

## Repository Contents
- `matrix_multiplication.c`: C implementation using contiguous 1D heap-allocated `double` arrays and `clock_gettime(CLOCK_MONOTONIC)`.
- `matrix_multiplication.py`: Pure Python implementation using nested lists and `time.perf_counter()` (with optional NumPy support if installed).
- `run_benchmarks.sh`: Automation script that compiles both C optimization levels and runs all matrix sizes.
- `slides.pdf`: Presentation slides summarizing the methodology, results, and systems-level analysis.

## Benchmarking Methodology
To avoid common benchmarking pitfalls, all experiments enforce:
1. **Identical Algorithm:** Standard triple-nested `i-j-k` matrix multiplication ($N^3$ multiply-accumulate operations).
2. **Internal High-Resolution Timers:** Uses `clock_gettime(CLOCK_MONOTONIC)` in C and `time.perf_counter()` in Python so interpreter startup and memory allocation overhead are excluded.
3. **Warm-Up & Median Aggregation:** Executes 1 untimed warm-up pass followed by 5 timed runs, recording the median runtime to filter out OS scheduling noise.
4. **Dead-Code Prevention:** Prints a checksum (`C[0]`) after timing so GCC `-O3` cannot optimize away the computation loop.

## How to Run
Run the full benchmark suite automatically:
```bash
chmod +x run_benchmarks.sh
./run_benchmarks.sh
```