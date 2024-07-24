#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive

# Tamaños de matriz y tamaños de bloque
matrix_sizes=(512 1024 2048 4096)

# Bucle para combinar los tamaños de matriz
for matrix_size in "${matrix_sizes[@]}"; do
    ./seqExe "$matrix_size" > "SalidaSecuencial/output_${matrix_size}.txt" 2> "SalidaSecuencial/errors_${matrix_size}.txt"
done