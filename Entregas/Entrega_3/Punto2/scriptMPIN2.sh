#!/bin/bash
#SBATCH -N 2
#SBATCH --exclusive
#SBATCH --tasks-per-node=8

# Tamaños de matriz y tamaños de bloque
matrix_sizes=(512 1024 2048 4096)

# Bucle anidado para combinar los tamaños de matriz y tamaños de bloque
for matrix_size in "${matrix_sizes[@]}"; do
    mpirun MPIexe "$matrix_size" > "SalidaMPI/output_${matrix_size}_N2.txt" 2> "SalidaMPI/errors_${matrix_size}_N2.txt"
done