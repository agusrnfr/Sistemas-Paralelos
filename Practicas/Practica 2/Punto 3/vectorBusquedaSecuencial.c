#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


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
    double *A;
    int N, T, X = 1, ocurrencias = 0;
    int i;
    int check = 1;
    double timetick;

    // Controla los argumentos al programa
    if ((argc != 2) || ((N = atoi(argv[1])) <= 0))
    {
        printf("\nUsar: %s n \n  n: Dimension del vector \n", argv[0]);
        exit(1);
    }

    // Aloca memoria para el vector
    A = (double *)malloc(sizeof(double) * N);

    // Inicializa el vector en 1
    for (i = 0; i < N; i++){
        A[i] = 1;
    }

    timetick = dwalltime();

    for (i = 0; i < N; i++)
    {
        if (A[i] == X)
        {
            ocurrencias++;
        }
    }

    double time = dwalltime() - timetick;

    printf("Elemento %d encontrado %d veces\n", X, ocurrencias);

    printf("Búsqueda de variable %d en vector de dimensión %d. Tiempo en segundos %f\n", X, N, time);

    // Verifica el resultado
    for (i = 0; i < N; i++)
        check = check && (ocurrencias == N);

    if (check)
    {
        printf("Busqueda correcta\n");
    }
    else
    {
        printf("Busqueda erronea\n");
    }

    free(A);
    return (0);
}