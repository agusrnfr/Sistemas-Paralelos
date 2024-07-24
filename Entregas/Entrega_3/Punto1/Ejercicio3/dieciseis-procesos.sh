#!/bin/bash
#SBATCH -N 2
#SBATCH --exclusive
#SBATCH --tasks-per-node=8

for N in 10000000 20000000 40000000; do
	output_file_blocking="Salidas/output_blocking_dieciseis_${N}.txt"
	error_file_blocking="Salidas/errors_blocking_dieciseis_${N}.txt"
	output_file_non_blocking="Salidas/output_non_blocking_dieciseis_${N}.txt"
	error_file_non_blocking="Salidas/errors_non_blocking_dieciseis_${N}.txt"
    	mpirun blocking "$N" > "$output_file_blocking" 2> "$error_file_blocking"
    	mpirun non-blocking "$N" > "$output_file_non_blocking" 2> "$error_file_non_blocking"
done