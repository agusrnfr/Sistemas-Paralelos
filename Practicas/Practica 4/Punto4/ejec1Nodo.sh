#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH --tasks-per-node=8

matrix_sizes=(512 1024 2048)

# Bucle anidado para combinar los tamaÃ±os de matriz y tamaÃ±os de bloque
for matrix_size in "${matrix_sizes[@]}"; do
    output_file="Salidas/output_unNodo_${matrix_size}.txt"
    error_file="Salidas/errors_unNodo_${matrix_size}.txt"
    mpirun paralelExe "$matrix_size" > "$output_file" 2> "$error_file"
done

for matrix_size in "${matrix_sizes[@]}"; do
    output_file="Salidas/output_unNodoG_${matrix_size}.txt"
    error_file="Salidas/errors_unNodoG_${matrix_size}.txt"
    mpirun paralelExe "$matrix_size" > "$output_file" 2> "$error_file"
done