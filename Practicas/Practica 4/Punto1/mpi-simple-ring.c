/*
** Sending simple, point-to-point messages.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

#define INITIAL 0

int main(int argc, char *argv[])
{
  int myrank;
  int size;
  int dest;
  int source;
  int tag = 0;
  MPI_Status status;
  char message[BUFSIZ];

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

  dest = (myrank + 1) % size;
  source = (myrank - 1) % size;


  if (myrank != INITIAL)
  {

    MPI_Recv(message, BUFSIZ, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
    printf("Soy el proceso %d. Mensaje recibido: %s\n", myrank, message);
    sprintf(message, "Hola Mundo! Soy el proceso %d!", myrank);
    MPI_Send(message, strlen(message) + 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
  }
  else
  {
    sprintf(message, "Hola Mundo! Soy el proceso %d!", myrank);
    MPI_Send(message, strlen(message) + 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    MPI_Recv(message, BUFSIZ, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
    printf("Soy el proceso %d. Mensaje recibido: %s\n", myrank, message);
  }


  MPI_Finalize();

  return EXIT_SUCCESS;
}