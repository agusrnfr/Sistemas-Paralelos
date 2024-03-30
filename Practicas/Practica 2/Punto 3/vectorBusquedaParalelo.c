#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

double *A;
int N, T, block_size, extra, X = 1, ocurrencias = 0;
pthread_mutex_t mutexOcurrencias;

void * busqueda_matriz (void * ptr) {
	
	int id = *(int *) ptr; // Convierte el puntero a entero y le asigna a id el valor apuntado

    int inicio = id * block_size;
    int fin = inicio + block_size;
    int local_ocurrencias = 0;

    if (id == T - 1)  // Si es el último hilo, se le asigna el resto de la división 
        fin += extra;

    for (int i = inicio; i < fin; i++){
        if (A[i] == X) {
            local_ocurrencias++;
        }
    }

    pthread_mutex_lock(&mutexOcurrencias); // Sección crítica
    ocurrencias+= local_ocurrencias;
    pthread_mutex_unlock(&mutexOcurrencias); // Fin sección crítica

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

    // Inicializa el vector en 1
    for (i = 0; i < N; i++){
        A[i] = 1;
    }

    pthread_attr_init(&attr);
    pthread_mutex_init(&mutexOcurrencias, NULL);

    // Crea los hilos
    for (i = 0; i < T; i++)
    {
        ids[i] = i;
        pthread_create(&threads[i], &attr, busqueda_matriz, &ids[i]);
    }

    // El hilo principal espera a que terminen todos los hilos de buscar la ocurrencia de X en el vector
    
    timetick = dwalltime();

    for (i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
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