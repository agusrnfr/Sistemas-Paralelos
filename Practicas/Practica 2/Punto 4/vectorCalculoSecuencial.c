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
    double *A;
    int N, maximo = -1, minimo= 1000000;
    long int suma = 0;
    int i;
    int check = 1;
    double timetick;

    // Controla los argumentos al programa
    if ((argc != 2) || ((N = atoi(argv[1])) <= 0))
    {
        printf("\nUsar: %s n t \n  n: Dimension del vector \n", argv[0]);
        exit(1);
    }
    
    // Aloca memoria para el vector
    A = (double *)malloc(sizeof(double) * N);

    // Inicializa los vectores en i
    for (i = 0; i < N; i++){
        A[i] = i;
    }
    
    timetick = dwalltime();

    for (int i = 0; i < N; i++){
        if (A[i] > maximo) {
            maximo = A[i];
        }
        if (A[i] < minimo) {
            minimo = A[i];
        }
        suma += A[i];
    }

    long int promedio = suma/N;

    double time = dwalltime() - timetick;

    printf("Máximo: %d\n", maximo);
    printf("Mínimo: %d\n", minimo);
    printf("Promedio: %ld\n", promedio);

    printf("Calculo de máximo, mínimo y promedio en vector de dimensión %d. Tiempo en segundos %f\n", N, time);

    // Verifica el resultado
    for (i = 0; i < N; i++)
        check = check && (maximo == N-1) && (minimo == 0) && (promedio == (N-1)/2);

    if (check)
    {
        printf("Calculo correcto\n");
    }
    else
    {
        printf("Calculo erroneo\n");
    }

    free(A);
    return (0);
}