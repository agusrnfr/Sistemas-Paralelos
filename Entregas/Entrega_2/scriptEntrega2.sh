#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive

# Tamaños de matriz y tamaños de bloque
matrix_sizes=(512 1024 2048 4096)
threads_sizes=(2 4 8)

# Bucle anidado para combinar los tamaños de matriz y tamaños de bloque
for matrix_size in "${matrix_sizes[@]}"; do
    output_file="Salidas/output_seq_${matrix_size}.txt"
    error_file="Salidas/errors_seq_${matrix_size}.txt"
    ./seqExe "$matrix_size" > "$output_file" 2> "$error_file"
    for thread_size in "${threads_sizes[@]}"; do
        output_file="Salidas/output_pt_${matrix_size}_TS${thread_size}.txt"
        error_file="Salidas/errors_pt_${matrix_size}_TS${thread_size}.txt"
        ./pthreadsExe "$matrix_size" "$thread_size" > "$output_file" 2> "$error_file"
        output_file="Salidas/output_mp_${matrix_size}_TS${thread_size}.txt"
        error_file="Salidas/errors_mp_${matrix_size}_TS${thread_size}.txt"
        ./mpExe "$matrix_size" "$thread_size" > "$output_file" 2> "$error_file"
    done
done