#!/bin/bash
#SBATCH -N 4
#SBATCH --exclusive
#SBATCH --tasks-per-node=1

# Tamaños de matriz y tamaños de bloque
matrix_sizes=(512 1024 2048 4096)

# Bucle anidado para combinar los tamaños de matriz y tamaños de bloque
for matrix_size in "${matrix_sizes[@]}"; do
    mpirun --bind-to none Hibridoexe "$matrix_size" "8" > "SalidaHibrido/output_${matrix_size}_N4.txt" 2> "SalidaHibrido/errors_${matrix_size}_N4.txt"
done