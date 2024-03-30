#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

double *A, *B, *INTERSEC;
int N, T, block_size, extra, k = 0;
pthread_mutex_t mutexIntersec;

void *intersec_vector(void *ptr)
{

    int id = *(int *)ptr; // Convierte el puntero a entero y le asigna a id el valor apuntado

    int inicio = id * block_size;
    int fin = inicio + block_size;

    if (id == T - 1) // Si es el último hilo, se le asigna el resto de la división
        fin += extra;

    for (int i = inicio; i < fin; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (A[i] == B[j])
            {
                pthread_mutex_lock(&mutexIntersec); // Sección crítica
                INTERSEC[k++] = A[i];
                pthread_mutex_unlock(&mutexIntersec); // Fin sección crítica
                break;
            }
        }
    }

    pthread_exit(0);
}

int comparar(const void *a, const void *b) {
    return (*(double*)a - *(double*)b);
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

    int i, j;
    int check = 1;
    double timetick;
    int ids[T];
    block_size = N / T;
    extra = N % T;
    pthread_attr_t attr;
    pthread_t threads[T];

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

    pthread_attr_init(&attr);
    pthread_mutex_init(&mutexIntersec, NULL);

    // Crea los hilos
    for (i = 0; i < T; i++)
    {
        ids[i] = i;
        pthread_create(&threads[i], &attr, intersec_vector, &ids[i]);
    }

    timetick = dwalltime();

    // El hilo principal espera a que terminen todos los hilos de buscar la intersección de los vectores

    for (i = 0; i < T; i++)
    {
        pthread_join(threads[i], NULL);
    }

    double time = dwalltime() - timetick;

    printf("Calculo de intersección en vector de dimensión %d. Tiempo en segundos %f\n", N, time);

    qsort(INTERSEC, k, sizeof(double), comparar); // Ordena el vector de intersección para la verificación

    /*
        for (i = 0; i < k; i++)
           printf("INTERSEC[%d] = %f\n", i, INTERSEC[i]);
    */

    // Verifica el resultado
    for (i = 0; i < k; i++){
        check = check && (INTERSEC[i] == i + 1);
    }

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