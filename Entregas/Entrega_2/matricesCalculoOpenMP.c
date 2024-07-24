#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>

#define BS 64

void initvalmat(double *mat, int n, double val, int orden);

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
    int N, T, i, j, k, offsetI, offsetJ;
    double timetick;
    double minA = INT_MAX, maxA = INT_MIN, minB = INT_MAX, maxB = INT_MIN;
    double promA = 0.0, promB = 0.0;
    double escalar = 0.0;
    double posA, posB;

    if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0))
    {
        printf("\nUsar: %s n t \n  n: Dimension de la matriz (nxn X nxn) \n  t: Cantidad de hilos \n", argv[0]);
        exit(1);
    }

    double size = N * N;
    omp_set_num_threads(T);

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
#pragma omp parallel private(i, j, k, offsetI, offsetJ, posA, posB)
    {
#pragma omp for reduction(min : minA, minB) reduction(max : maxA, maxB) reduction(+ : promA, promB) schedule(static) //No se necesita el nowait porque se necesita que todos los hilos terminen para calcular el escalar
        for (i = 0; i < N; i++)
        {
            offsetI = i * N;
            for (j = 0; j < N; j++)
            {
                offsetJ = j * N;
                posA = A[offsetI + j];
                posB = B[offsetI + j];

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

#pragma omp single // Se calcula el escalar
        {
            promA = promA / (size);
            promB = promB / (size);

            escalar = (maxA * maxB - minA * minB) / (promA * promB);
        }

        // Se realiza la multiplicacion de matrices A y B
        #pragma omp for nowait schedule(static) // Se utiliza nowait para que los hilos no esperen a que todos terminen para multiplicar las matrices C y D
        for (i = 0; i < N; i += BS)
        {
            offsetI = i * N;
            for (j = 0; j < N; j += BS)
            {
                offsetJ = j * N;
                for (k = 0; k < N; k += BS)
                {
                    blkmul(&A[offsetI + k], &B[offsetJ + k], &resultMatriz[offsetI + j], N, BS);
                }
            }
        }

        // Se realiza la multiplicacion de matrices C y D
        #pragma omp for schedule(static) //No se necesita el nowait porque se necesita que todos los hilos terminen para sumar los resultados
        for (i = 0; i < N; i += BS)
        {
            offsetI = i * N;
            for (j = 0; j < N; j += BS)
            {
                offsetJ = j * N;
                for (k = 0; k < N; k += BS)
                {
                    blkmul(&C[offsetI + k], &D[offsetJ + k], &R[offsetI + j], N, BS);
                }
            }
        }

        // Se suman los resultados de las multiplicaciones de matrices y al mismo tiempo se multiplica por el escalar resultMatriz
        #pragma omp for nowait schedule(static)
        for (i = 0; i < N; i++)
        {
            offsetI = i * N;
            for (j = 0; j < N; j++)
            {
                R[offsetI + j] += resultMatriz[offsetI + j] * escalar;
            }
        }
    }
    double workTime = dwalltime() - timetick;

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
    int i, j, offsetI;

    if (orden == 0)
    {
        for (i = 0; i < n; i++)
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
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < n; j++)
            {
                mat[j * n + i] = val;
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