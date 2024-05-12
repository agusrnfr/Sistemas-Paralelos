#!/bin/bash
#SBATCH -N 2
#SBATCH --exclusive
#SBATCH --tasks-per-node=8

matrix_sizes=(16384 32768 65536)

# Bucle anidado para combinar los tamaÃ±os de matriz y tamaÃ±os de bloque
for matrix_size in "${matrix_sizes[@]}"; do
    output_file="Salidas/output_dosNodos_${matrix_size}.txt"
    error_file="Salidas/errors_dosNodos_${matrix_size}.txt"
    mpirun paralelExe "$matrix_size" > "$output_file" 2> "$error_file"
done
