#!/bin/bash
#SBATCH -N 2
#SBATCH --exclusive
#SBATCH --tasks-per-node=1

matrix_sizes=(512 1024 2048)

# Bucle anidado para combinar los tamaÃ±os de matriz y tamaÃ±os de bloque
for matrix_size in "${matrix_sizes[@]}"; do
    output_file="Salidas/output_dosNodos_${matrix_size}.txt"
    error_file="Salidas/errors_dosNodos_${matrix_size}.txt"
    mpirun --bind-to none paralelExe "$matrix_size" "64" "8" > "$output_file" 2> "$error_file"
done
