#!/usr/bin/env bash
set -e

echo "Compiling C benchmarks..."
gcc -O0 -Wall matrix_multiplication.c -o matrix_multiplication_O0
gcc -O3 -Wall matrix_multiplication.c -o matrix_multiplication_O3

for N in 100 250 400 500; do
    echo "=========================================="
    echo "Matrix Size: ${N} x ${N} ($((N*N*N)) ops)"
    echo "=========================================="
    ./matrix_multiplication_O0 $N
    ./matrix_multiplication_O3 $N
    python3 matrix_multiplication.py $N
done

echo "=========================================="
echo "Bonus: C (-O3) Only at N = 1000 (1B ops)"
echo "=========================================="
./matrix_multiplication_O3 1000