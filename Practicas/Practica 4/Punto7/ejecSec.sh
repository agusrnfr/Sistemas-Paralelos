#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive

matrix_sizes=(512 1024 2048)

# Bucle anidado para combinar los tamaÃ±os de matriz y tamaÃ±os de bloque
for matrix_size in "${matrix_sizes[@]}"; do
    output_file="Salidas/output_seq_${matrix_size}.txt"
    error_file="Salidas/errors_seq_${matrix_size}.txt"
    ./seqExe "$matrix_size" > "$output_file" 2> "$error_file"
done