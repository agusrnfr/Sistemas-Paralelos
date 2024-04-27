#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Dimension por defecto de las matrices
int N, T, block_size;
double *A, *B, *C;

void *mult_matriz(void *ptr)
{
  int suma;
  int id = *(int *)ptr;
  int inicio = id * block_size;
  int fin = inicio + block_size;

  for (int i = inicio; i < fin; i++) {
    for (int j = 0; j < N; j++) {
      suma = 0;
      for (int k = 0; k < N; k++) {
        suma += A[i*N+k] * B[j*N+k];
      }
      
      C[i * N + j] = suma;
    }
  }
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
    printf("\nUsar: %s n t \n  n: Dimension de la matriz (nxn X nxn) \n  t: Cantidad de hilos \n", argv[0]);
    exit(1);
  }
  int i, j;
  int check = 1;
  double timetick;
  int ids[T];
  block_size = N / T;
  pthread_attr_t attr;
  pthread_t threads[T];

  // Aloca memoria para las matrices
  A = (double *)malloc(sizeof(double) * N * N);
  B = (double *)malloc(sizeof(double) * N * N);
  C = (double *)malloc(sizeof(double) * N * N);

  // Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en 1
  for (i = 0; i < N; i++)
  {
    for (j = 0; j < N; j++)
    {
      A[i * N + j] = 1;
      B[j * N + i] = 1;
    }
  }

  pthread_attr_init(&attr);

  // Crea los hilos
  for (i = 0; i < T; i++)
  {
    ids[i] = i;
    pthread_create(&threads[i], &attr, mult_matriz, &ids[i]);
  }

  // El hilo principal espera a que terminen todos los hilos de multiplicar la parte correspondiente de la matriz

  timetick = dwalltime();

  for (i = 0; i < T; i++)
  {
    pthread_join(threads[i], NULL);
  }

  printf("Multiplicacion de matrices de %dx%d. Tiempo en segundos %f\n", N, N, dwalltime() - timetick);

  // Verifica el resultado
  for (i = 0; i < N; i++)
  {
    for (j = 0; j < N; j++)
    {
      check = check && (C[i * N + j] == N);
    }
  }

  if (check)
  {
    printf("Multiplicacion de matrices resultado correcto\n");
  }
  else
  {
    printf("Multiplicacion de matrices resultado erroneo\n");
  }

  free(A);
  free(B);
  free(C);
  return (0);
}
