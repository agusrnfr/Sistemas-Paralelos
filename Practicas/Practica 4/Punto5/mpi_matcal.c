#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define COORDINATOR 0

void initvalmat(double *mat, int n, double val, int orden);

void matmulblks(double *a, double *b, double *c, int n, int bs, int stripSize);

void blkmul(double *ablk, double *bblk, double *cblk, int n, int bs);

int main(int argc, char *argv[])
{
	int i, j, numProcs, rank, stripSize, check = 1;
	double *a, *b, *c, *d, *e, *f, *r1, *r2, *r3;
	MPI_Status status;
	double commTimes[6], maxCommTimes[6], minCommTimes[6], commTime, totalTime;
	int n = atoi(argv[1]);
	int bs = atoi(argv[2]);

	/* lee parametros de la linea de comando */
	if ((argc != 3) || (n <= 0) || (bs <= 0) || ((n % bs) != 0))
	{
		printf("\nParámetros inválidos. Debe utilizar %s N BS (N debe ser múltiplo de BS)\n", argv[0]);
		exit(1);
	}

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (n % numProcs != 0)
	{
		printf("El tamaño de la matriz debe ser multiplo del numero de procesos.\n");
		exit(1);
	}

	/* calcular porcion de cada worker */
	stripSize = n / numProcs;

	/* reservar memoria */
	if (rank == COORDINATOR)
	{
		a = (double *)malloc(sizeof(double) * n * n);
		c = (double *)malloc(sizeof(double) * n * n);
		e = (double *)malloc(sizeof(double) * n * n);
		r1 = (double *)malloc(sizeof(double) * n * n);
		r2 = (double *)malloc(sizeof(double) * n * n);
		r3 = (double *)malloc(sizeof(double) * n * n);

	}
	else
	{
		a = (double *)malloc(sizeof(double) * n * stripSize);
		c = (double *)malloc(sizeof(double) * n * stripSize);
		e = (double *)malloc(sizeof(double) * n * stripSize);
		r1 = (double *)malloc(sizeof(double) * n * stripSize);
		r2 = (double *)malloc(sizeof(double) * n * stripSize);
		r3 = (double *)malloc(sizeof(double) * n * stripSize);
	}

	b = (double *)malloc(sizeof(double) * n * n);
	d = (double *)malloc(sizeof(double) * n * n);
	f = (double *)malloc(sizeof(double) * n * n);

	// inicializar datos
	if (rank == COORDINATOR)
	{

		initvalmat(a, n, 1, 0);
		initvalmat(c, n, 1, 0);
		initvalmat(e, n, 1, 0);
		initvalmat(b, n, 1, 1);
		initvalmat(d, n, 1, 1);
		initvalmat(f, n, 1, 1);
		initvalmat(r1, n, 0, 0);
		initvalmat(r2, n, 0, 0);
		initvalmat(r3, n, 0, 0);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	commTimes[0] = MPI_Wtime();

	/* distribuir datos*/
	MPI_Scatter(a, n * stripSize, MPI_DOUBLE, a, n * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Scatter(c, n * stripSize, MPI_DOUBLE, c, n * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Scatter(e, n * stripSize, MPI_DOUBLE, e, n * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Bcast(b, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Bcast(d, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Bcast(f, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

	commTimes[1] = MPI_Wtime();

	/* computar multiplicaciones parciales */
	matmulblks(a, b, r1, n, bs, stripSize);
	matmulblks(c, d, r2, n, bs, stripSize);
	matmulblks(e, f, r3, n, bs, stripSize);

	commTimes[2] = MPI_Wtime();

	/* recolectar resultados parciales */
	MPI_Gather(r1, n * stripSize, MPI_DOUBLE, r1, n * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Gather(r2, n * stripSize, MPI_DOUBLE, r2, n * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Gather(r3, n * stripSize, MPI_DOUBLE, r3, n * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

	/* distribuir resultados parciales */
	MPI_Scatter(r1, n * stripSize, MPI_DOUBLE, r1, n * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Scatter(r2, n * stripSize, MPI_DOUBLE, r2, n * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Scatter(r3, n * stripSize, MPI_DOUBLE, r3, n * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

	commTimes[3] = MPI_Wtime();

	/* computar sumas parciales */
	for (int i = 0; i < stripSize; i++)
	{
		for (int j = 0; j < n; j++)
		{
			r1[i * n + j] += r2[i * n + j] + r3[i * n + j];
		}
	}

	commTimes[4] = MPI_Wtime();

	/* recolectar resultados finales */
	MPI_Gather(r1, n * stripSize, MPI_DOUBLE, r1, n * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

	commTimes[5] = MPI_Wtime();

	MPI_Reduce(commTimes, minCommTimes, 6, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(commTimes, maxCommTimes, 6, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

	MPI_Finalize();

	if (rank == COORDINATOR)
	{

		// Check results
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				check = check && (r1[i * n + j] == n * 3);

		if (check)
		{
			printf("Calculo de matrices resultado correcto\n");
		}
		else
		{
			printf("Calculo de matrices resultado erroneo\n");
		}

		totalTime = maxCommTimes[5] - minCommTimes[0];
		commTime = (maxCommTimes[1] - minCommTimes[0]) + (maxCommTimes[3] - minCommTimes[2]) + (maxCommTimes[5] - minCommTimes[4]);

		printf("Calculo de matrices (N=%d)\tTiempo total=%lf\tTiempo comunicacion=%lf\n", n, totalTime, commTime);
	}

	free(a);
	free(b);
	free(c);
	free(d);
	free(e);
	free(f);
	free(r1);
	free(r2);
	free(r3);
	
	return 0;
}


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

void matmulblks(double *a, double *b, double *c, int n, int bs, int stripSize)
{
    int i, j, k, offsetI, offsetJ;

    for (i = 0; i < stripSize; i += bs)
    {
        offsetI = i * n;
        for (j = 0; j < n; j += bs)
        {
            offsetJ = j * n;
            for (k = 0; k < n; k += bs)
            {
                blkmul(&a[offsetI + k], &b[offsetJ + k], &c[offsetI + j], n, bs);
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