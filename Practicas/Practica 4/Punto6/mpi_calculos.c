#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <limits.h>
#include <mpi.h>

#define COORDINATOR 0

int main(int argc, char *argv[])
{
    int i, n, numProcs, rank, stripSize;
    double *a;
    MPI_Status status;
    double commTimes[5], maxCommTimes[5], minCommTimes[5], commTime, totalTime;
    int local_sum = 0, local_min = INT_MAX, local_max = INT_MIN, sum = 0, min = INT_MAX, max = INT_MIN;
    double prom;

    if ((argc != 2) || ((n = atoi(argv[1])) <= 0))
    {
        printf("\nUsar: %s size \n  size: Tamaño del vector\n", argv[0]);
        exit(1);
    }

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (n % numProcs != 0)
    {
        printf("El tamaño de la matriz debe ser multiplo del numero de procesos.\n");
        exit(1);
    }

    stripSize = n / numProcs;

    if (rank == COORDINATOR)
    {
        a = (double *)malloc(sizeof(double) * n);
    }
    else
    {
        a = (double *)malloc(sizeof(double) * stripSize);
    }

    if (rank == COORDINATOR)
    {
        for (i = 0; i < n; i++)
            a[i] = i;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    commTimes[0] = MPI_Wtime();

    MPI_Scatter(a, stripSize, MPI_DOUBLE, a, stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

    commTimes[1] = MPI_Wtime();

    for (i = 0; i < stripSize; i++)
    {

        if (a[i] < local_min)
        {
            local_min = a[i];
        }

        if (a[i] > local_max)
        {
            local_max = a[i];
        }

        local_sum += a[i];
    }

    commTimes[2] = MPI_Wtime();

    MPI_Reduce(&local_min, &min, 1, MPI_INT, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(&local_max, &max, 1, MPI_INT, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(&local_sum, &sum, 1, MPI_INT, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);

    commTimes[3] = MPI_Wtime();

    if (rank == COORDINATOR)
    {

        prom = sum / n;
    }

    commTimes[4] = MPI_Wtime();

    MPI_Reduce(commTimes, minCommTimes, 5, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(commTimes, maxCommTimes, 5, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

    MPI_Finalize();

    // CHECK

    if (rank == COORDINATOR)
    {

        if ((max == n - 1) && (min == 0) && (prom == (n - 1) / 2))
        {
            printf("Resultado correcto\n");
        }
        else
        {
            printf("Resultado incorrecto\n");
        }

        totalTime = maxCommTimes[4] - minCommTimes[0];
        commTime = (maxCommTimes[1] - minCommTimes[0]) + (maxCommTimes[3] - minCommTimes[2]);

        printf("Minimo: %d\n", min);
        printf("Maximo: %d\n", max);
        printf("Promedio: %f\n", prom);

        printf("Calculo de vector (N=%d)\tTiempo total=%lf\tTiempo comunicacion=%lf\n", n, totalTime, commTime);
    }

    free(a);

    return 0;
}