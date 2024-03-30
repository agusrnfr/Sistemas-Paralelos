#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>

double *A;
int N, T, block_size, extra, maximo = INT_MIN, minimo = INT_MAX;
long int suma = 0;
sem_t semaforo;

void *calc_matriz(void *ptr)
{

    int id = *(int *)ptr; // Convierte el puntero a entero y le asigna a id el valor apuntado
    int inicio = id * block_size;
    int fin = inicio + block_size;
    int local_max = INT_MIN;
    int local_min = INT_MAX;
    int local_sum = 0;

    if (id == T - 1) // Si es el último hilo, se le asigna el resto de la división
        fin += extra;

    for (int i = inicio; i < fin; i++)
    {
        if (A[i] > maximo)
        {
            maximo = A[i];
        }
        if (A[i] < minimo)
        {
            minimo = A[i];
        }
        local_sum += A[i];
    }

    sem_wait(&semaforo);
    suma += local_sum;
    if (local_max > maximo)
    {
        maximo = local_max;
    }
    if (local_min < minimo)
    {
        minimo = local_min;
    }
    sem_post(&semaforo);

    pthread_exit(0);
}

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
    // Controla los argumentos al programa
    if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0))
    {
        printf("\nUsar: %s n t \n  n: Dimension del vector \n  t: Cantidad de hilos \n", argv[0]);
        exit(1);
    }

    int i;
    int check = 1;
    double timetick;
    int ids[T];
    block_size = N / T;
    extra = N % T;
    pthread_attr_t attr;
    pthread_t threads[T];

    // Aloca memoria para el vector
    A = (double *)malloc(sizeof(double) * N);

    // Inicializa el vector en i
    for (i = 0; i < N; i++)
    {
        A[i] = i;
    }

    pthread_attr_init(&attr);
    sem_init(&semaforo, 0, 1);

    // Crea los hilos
    for (i = 0; i < T; i++)
    {
        ids[i] = i;
        pthread_create(&threads[i], &attr, calc_matriz, &ids[i]);
    }

    // El hilo principal espera a que terminen todos los hilos terminen de realizar los cálculos

    timetick = dwalltime();

    for (i = 0; i < T; i++)
    {
        pthread_join(threads[i], NULL);
    }

    long int promedio = suma / N;

    double time = dwalltime() - timetick;

    printf("Máximo: %d\n", maximo);
    printf("Mínimo: %d\n", minimo);
    printf("Promedio: %ld\n", promedio);

    printf("Calculo de máximo, mínimo y promedio en vector de dimensión %d. Tiempo en segundos %f\n", N, time);

    // Verifica el resultado
    for (i = 0; i < N; i++)
        check = check && (maximo == N - 1) && (minimo == 0) && (promedio == (N - 1) / 2);

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