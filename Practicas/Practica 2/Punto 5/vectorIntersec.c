#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Para calcular tiempo
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
    double *A, *B, *INTERSEC;
    int N;
    int i, j, k = 0;
    int check = 1;
    double timetick;

    // Controla los argumentos al programa
    if ((argc != 2) || ((N = atoi(argv[1])) <= 0))
    {
        printf("\nUsar: %s n t \n  n: Dimension del vector \n", argv[0]);
        exit(1);
    }

    // Aloca memoria para los vectores
    A = (double *)malloc(sizeof(double) * N);
    B = (double *)malloc(sizeof(double) * N);
    INTERSEC = (double *)malloc(sizeof(double) * N);

    // Inicializa los vectores en i
    for (i = 0; i < N; i++)
    {
        A[i] = i + 1;
        B[i] = i;
    }

    /*
        srand(time(NULL));

        for (i = 0; i < N; i++){
            A[i] = rand() % 100;
            B[i] = rand() % 100;
        }

        for (i = 0; i < N; i++)
            printf("A[%d] = %f, B[%d] = %f\n", i, A[i], i, B[i]);

    */

    timetick = dwalltime();

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            if (A[i] == B[j])
            {
                INTERSEC[k++] = A[i];
                break;
            }
        }
    }

    double time = dwalltime() - timetick;

    printf("Calculo de intersección en vector de dimensión %d. Tiempo en segundos %f\n", N, time);

    /*
        for (i = 0; i < k; i++)
           printf("INTERSEC[%d] = %f\n", i, INTERSEC[i]);
    */

    // Verifica el resultado
    for (i = 0; i < k; i++)
        check = check && (INTERSEC[i] == i + 1);

    if (check)
    {
        printf("Intersección correcta\n");
    }
    else
    {
        printf("Intersección erronea\n");
    }

    free(A);
    free(B);
    free(INTERSEC);
    return (0);
}