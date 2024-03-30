#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

double *A, *B, *C;
int N, T, block_size, extra;

void * suma_matriz (void * ptr) {
	
	int id = *(int *) ptr; // Convierte el puntero a entero y le asigna a id el valor apuntado

    int inicio = id * block_size;
    int fin = inicio + block_size;

    if (id == T - 1)  // Si es el último hilo, se le asigna el resto de la división 
        fin += extra;

    for (int i = inicio; i < fin; i++){
        C[i] = A[i] + B[i];
    }

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

void print_m () {
    for (int i = 0; i < N; i++) {
        printf("Valor en posicion %d = %f\n", i, C[i]);
    }
}

int main(int argc, char *argv[])
{
    // Controla los argumentos al programa
    if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0))
    {
        printf("\nUsar: %s n t \n  n: Dimension de la matriz (nxn X nxn) \n  t: Cantidad de hilos \n", argv[0]);
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

    // Aloca memoria para los vectores
    A = (double *)malloc(sizeof(double) * N);
    B = (double *)malloc(sizeof(double) * N);
    C = (double *)malloc(sizeof(double) * N);

    // Inicializa los vectores en 1, el resultado será una vector con todos sus valores en 2
    for (i = 0; i < N; i++){
        A[i] = 1;
        B[i] = 1;
    }

    pthread_attr_init(&attr);

    // Crea los hilos
    for (i = 0; i < T; i++)
    {
        ids[i] = i;
        pthread_create(&threads[i], &attr, suma_matriz, &ids[i]);
    }

    // El hilo principal espera a que terminen todos los hilos de sumar la parte correspondiente del vector
    
    timetick = dwalltime();

    for (i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
	}

    double time = dwalltime() - timetick;

    //print_m(); // Imprime el vector resultante

    printf("Suma de vectores de dimensión %d. Tiempo en segundos %f\n", N, time);

    // Verifica el resultado
    for (i = 0; i < N; i++)
        check = check && (C[i] == 2);

    if (check)
    {
        printf("Suma de vectores resultado correcto\n");
    }
    else
    {
        printf("Suma de vectores resultado erroneo\n");
    }

    free(A);
    free(B);
    free(C);
    return (0);
}