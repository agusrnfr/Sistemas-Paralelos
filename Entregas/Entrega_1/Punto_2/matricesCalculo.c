#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

void initvalmat(double *mat, int n, double val, int orden);

void matmulblks(double *a, double *b, double *c, int n, int bs);

void blkmul(double *ablk, double *bblk, double *cblk, int n, int bs);

double dwalltime()
{
    double sec;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    sec = tv.tv_sec + tv.tv_usec / 1000000.0;
    return sec;
}

int main(int argc, char *argv[])
{
    double *A, *B, *C, *D, *R, *resultMatriz;
    int N, BS, i, j, offsetI, offsetJ;
    double timetick;
    double minA = INT_MAX, maxA = INT_MIN, minB = INT_MAX, maxB = INT_MIN;
    double promA = 0.0, promB = 0.0;
    double escalar = 0.0;
    double posA, posB;

    N = atoi(argv[1]);
    BS = atoi(argv[2]);

    if ((argc != 3) || (N <= 0) || (BS <= 0) || ((N % BS) != 0))
    {
        printf("\nParámetros inválidos. Debe utilizar %s N BS (N debe ser múltiplo de BS)\n", argv[0]);
        exit(1);
    }

    double size = N * N;

    A = (double *)malloc(size * sizeof(double));
    B = (double *)malloc(size * sizeof(double));
    C = (double *)malloc(size * sizeof(double));
    D = (double *)malloc(size * sizeof(double));
    R = (double *)malloc(size * sizeof(double));
    resultMatriz = (double *)malloc(size * sizeof(double));

    // Inicializa las matrices A, B, C y D en 1
    initvalmat(A, N, 1.0, 0); // Orden 0 para que se inicialice en orden de filas
    initvalmat(B, N, 1.0, 1); // Orden 1 para que se inicialice en orden de columnas
    initvalmat(C, N, 1.0, 0); // Orden 0 para que se inicialice en orden de filas
    initvalmat(D, N, 1.0, 1); // Orden 1 para que se inicialice en orden de columnas
    initvalmat(R, N, 0.0, 0);
    initvalmat(resultMatriz, N, 0.0, 0);

    timetick = dwalltime();

    // Se calculan los minimos, maximos y promedios de las matrices A y B
    for (i = 0; i < N; i++)
    {
        offsetI = i * N;
        for (j = 0; j < N; j++)
        {
            offsetJ = j * N;
            posA = A[offsetI + j];
            posB = B[offsetJ + i];

            if (posA < minA)
            {
                minA = posA;
            }

            if (posA > maxA)
            {
                maxA = posA;
            }

            promA += posA;

            if (posB < minB)
            {
                minB = posB;
            }

            if (posB > maxB)
            {
                maxB = posB;
            }

            promB += posB;
        }
    }

    promA = promA / (size);
    promB = promB / (size);

    // Se calcula el escalar
    escalar = (maxA * maxB - minA * minB) / (promA * promB);

    // Se realiza la multiplicacion de matrices A y B 
    matmulblks(A, B, resultMatriz, N, BS);

    // Se realiza la multiplicacion de matrices C y D 
    matmulblks(C, D, R, N, BS);

    // Se suman los resultados de las multiplicaciones de matrices y al mismo tiempo se multiplica por el escalar resultMatriz
    for (i = 0; i < N; i++)
    {
        offsetI = i * N;
        for (j = 0; j < N; j++)
        {
            R[offsetI + j] += resultMatriz[offsetI + j] * escalar;
        }
    }

    double workTime = dwalltime() - timetick;

    printf("Tiempo en segundos %f\n", workTime);

    free(A);
    free(B);
    free(C);
    free(D);
    free(R);
    free(resultMatriz);

    return (0);
}

// Inicializa una matriz de nxn con un valor val
void initvalmat(double *mat, int n, double val, int orden)
{
    int i, j;

    if (orden == 0)
    {
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < n; j++)
            {
                mat[i * n + j] = val;
            }
        }
    }
    else
    {
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < n; j++)
            {
                mat[j * n + i] = val;
            }
        }
    }
}

// Realiza la multiplicacion de matrices en bloques de tamaño bs
void matmulblks(double *a, double *b, double *c, int n, int bs)
{
    int i, j, k, offsetI, offsetJ;

    for (i = 0; i < n; i += bs)
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

    for (i = 0; i < bs; i++)
    {
        offsetI = i * n;
        for (j = 0; j < bs; j++)
        {
            offsetJ = j * n;
            for (k = 0; k < bs; k++)
            {
                cblk[offsetI + j] += ablk[offsetI + k] * bblk[offsetJ + k];
            }

        }
    }
}