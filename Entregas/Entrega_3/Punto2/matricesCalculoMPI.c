#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <mpi.h>

#define BS 64
#define COORDINATOR 0

void initvalmat(double *mat, int n, double val, int orden, int stripSize);

void matmulblks(double *a, double *b, double *c, int n, int bs, int stripSize);

void blkmul(double *ablk, double *bblk, double *cblk, int n, int bs);

int main(int argc, char *argv[])
{
    double *A, *B, *C, *D, *R, *resultMatriz;
    int N, bs, i, j, offsetI, offsetJ, numProcs, rank, stripSize;
    double local_min[2] = {INT_MAX, INT_MAX}, local_max[2] = {INT_MIN, INT_MIN}, min[2] = {INT_MAX, INT_MAX}, max[2] = {INT_MIN, INT_MIN};
    double local_prom[2] = {0,0}, prom[2] = {0,0};
    double escalar = 0.0;
    double posA, posB;
    MPI_Status status;
    double commTimes[8], maxCommTimes[8], minCommTimes[8], commTime, totalTime;

    N = atoi(argv[1]);

    if ((argc != 2) || (N <= 0) || (N % 64 != 0))
    {
        printf("\nParámetros inválidos. Debe utilizar %s N (N debe ser múltiplo de 64)\n", argv[0]);
        exit(1);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (N % numProcs != 0)
    {
        printf("El tamaño de la matriz debe ser múltiplo del numero de procesos.\n");
        exit(1);
    }

    double size = N * N;

    stripSize = N / numProcs;

    double sizeWorker = N * stripSize;

    bs = (stripSize < BS ? stripSize : BS);

    if (rank == COORDINATOR)
    {
        A = (double *)malloc(size * sizeof(double));
        C = (double *)malloc(size * sizeof(double));
        R = (double *)malloc(size * sizeof(double));
        resultMatriz = (double *)malloc(size * sizeof(double));
    }
    else
    {
        A = (double *)malloc(sizeWorker * sizeof(double));
        C = (double *)malloc(sizeWorker * sizeof(double));
        R = (double *)malloc(sizeWorker * sizeof(double));
        resultMatriz = (double *)malloc(sizeWorker * sizeof(double));
    }

    B = (double *)malloc(size * sizeof(double));
    D = (double *)malloc(size * sizeof(double));

    if (rank == COORDINATOR)
    {
        // Inicializa las matrices A, B, C y D en 1
        initvalmat(A, N, 1.0, 0, N); // Orden 0 para que se inicialice en orden de filas
        initvalmat(B, N, 1.0, 1, N); // Orden 1 para que se inicialice en orden de columnas
        initvalmat(C, N, 1.0, 0, N); // Orden 0 para que se inicialice en orden de filas
        initvalmat(D, N, 1.0, 1, N); // Orden 1 para que se inicialice en orden de columnas
        initvalmat(R, N, 0.0, 0, N);
        initvalmat(resultMatriz, N, 0.0, 0, N);
    } else {
        initvalmat(R, N, 0.0, 0, stripSize);
        initvalmat(resultMatriz, N, 0.0, 0, stripSize);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    commTimes[0] = MPI_Wtime();

    // Se reparten las matrices
    MPI_Scatter(A, sizeWorker, MPI_DOUBLE, A, sizeWorker, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    MPI_Scatter(C, sizeWorker, MPI_DOUBLE, C, sizeWorker, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

    MPI_Bcast(B, size, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
    MPI_Bcast(D, size, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

    commTimes[1] = MPI_Wtime();

    // Se calculan los minimos, maximos y promedios de las matriz A
    for (i = 0; i < stripSize; i++)
    {
        offsetI = i * N;
        for (j = 0; j < N; j++)
        {
            posA = A[offsetI + j];

            if (posA < local_min[0])
            {
                local_min[0] = posA;
            }

            if (posA > local_max[0])
            {
                local_max[0] = posA;
            }

            local_prom[0] += posA;
        }
    }

    // Se calculan los minimos, maximos y promedios de las matriz B
    int inicio = stripSize * rank;
    int fin = inicio + stripSize;
    for (i = inicio; i < fin; i++)
    {
        offsetI = i * N;
        for (j = 0; j < N; j++)
        {
            posB = B[offsetI + j];

            if (posB < local_min[1])
            {
                local_min[1] = posB;
            }

            if (posB > local_max[1])
            {
                local_max[1] = posB;
            }

            local_prom[1] += posB;
        }
    }

    commTimes[2] = MPI_Wtime();

    // Se recolectan los minimos, maximos y promedios
    MPI_Reduce(&local_min, &min, 2, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(&local_max, &max, 2, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(&local_prom, &prom, 2, MPI_DOUBLE, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);

    commTimes[3] = MPI_Wtime();

    // Se calcula el escalar
    if (rank == COORDINATOR)
    {
        prom[0] = prom[0] / (size);
        prom[1] = prom[1] / (size);

        escalar = (max[0] * max[1] - min[0] * min[1]) / (prom[0] * prom[1]);
    }

    // Se realiza la multiplicacion de matrices A y B
    matmulblks(A, B, resultMatriz, N, bs, stripSize);

    // Se realiza la multiplicacion de matrices C y D
    matmulblks(C, D, R, N, bs, stripSize);

    // Se transmite el escalar
    commTimes[4] = MPI_Wtime();

    MPI_Bcast(&escalar, 1, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

    commTimes[5] = MPI_Wtime();

    // Se suman los resultados de las multiplicaciones de matrices y al mismo tiempo se multiplica por el escalar resultMatriz
    for (i = 0; i < stripSize; i++)
    {
        offsetI = i * N;
        for (j = 0; j < N; j++)
        {
            R[offsetI + j] += resultMatriz[offsetI + j] * escalar;
        }
    }

    // Se recolectan los resultados
    commTimes[6] = MPI_Wtime();

    MPI_Gather(R, sizeWorker, MPI_DOUBLE, R, sizeWorker, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

    commTimes[7] = MPI_Wtime();

    MPI_Reduce(commTimes, minCommTimes, 8, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
    MPI_Reduce(commTimes, maxCommTimes, 8, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

    MPI_Finalize();

    if (rank == COORDINATOR)
    {
        /*  
        // Se imprime el resultado de la matriz R
        printf("Resultado de la matriz R\n");
        for (i = 0; i < N; i++)
        {
            offsetI = i * N;
            for (j = 0; j < N; j++)
            {
                printf("%f ", R[offsetI + j]);
            }
            printf("\n");
        }
        */
    

        totalTime = maxCommTimes[7] - minCommTimes[0];
        commTime = (maxCommTimes[1] - minCommTimes[0]) + (maxCommTimes[3] - minCommTimes[2]) + (maxCommTimes[5] - minCommTimes[4]) + (maxCommTimes[7] - minCommTimes[6]);

        printf("Calculo de vector (N=%d)\tTiempo total=%lf\tTiempo comunicacion=%lf\n", N, totalTime, commTime);
    }

    free(A);
    free(B);
    free(C);
    free(D);
    free(R);
    free(resultMatriz);

    return (0);
}

// Inicializa una matriz de nxn con un valor val
void initvalmat(double *mat, int n, double val, int orden, int stripSize)
{
    int i, j, offsetI;

    if (orden == 0)
    {
        for (i = 0; i < stripSize; i++)
        {
            offsetI = i * n;
            for (j = 0; j < n; j++)
            {
                mat[offsetI + j] = val;
            }
        }
    }
    else
    {
        for (i = 0; i < stripSize; i++)
        {
            for (j = 0; j < n; j++)
            {
                mat[j * n + i] = val;
            }
        }
    }
}

// Realiza la multiplicacion de matrices en bloques de tamaño bs
void matmulblks(double *a, double *b, double *c, int n, int bs, int stripSize)
{
    int i, j, k, offsetI, offsetJ;

    for (i = 0; i < stripSize; i += bs)
    {
        offsetI = i * n;
        for (j = 0; j < n; j += bs)
        {
            offsetJ = j * n;
            for (k = 0; k < n; k += bs)
            {
                blkmul(&a[offsetI + k], &b[offsetJ + k], &c[offsetI + j], n, bs);
            }
        }
    }
}

void blkmul(double *ablk, double *bblk, double *cblk, int n, int bs)
{
    int i, j, k, offsetI, offsetJ;
    double suma;

    for (i = 0; i < bs; i++)
    {
        offsetI = i * n;
        for (j = 0; j < bs; j++)
        {
            suma = 0;
            offsetJ = j * n;
            for (k = 0; k < bs; k++)
            {
                suma += ablk[offsetI + k] * bblk[offsetJ + k];
            }

            cblk[offsetI + j] += suma;
        }
    }
}