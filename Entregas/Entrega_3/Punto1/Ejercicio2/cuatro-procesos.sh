#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH --tasks-per-node=4

output_file="Salidas/output_blocking_cuatro.txt"
error_file="Salidas/errors_blocking_cuatro.txt"

mpirun blocking > "$output_file" 2> "$error_file"

output_file="Salidas/output_non_blocking_cuatro.txt"
error_file="Salidas/errors_non_blocking_cuatro.txt"

mpirun non-blocking > "$output_file" 2> "$error_file"

output_file="Salidas/output_non_blocking_no_wait_cuatro.txt"
error_file="Salidas/errors_non_blocking_no_wait_cuatro.txt"

mpirun non-blocking-no-wait > "$output_file" 2> "$error_file"