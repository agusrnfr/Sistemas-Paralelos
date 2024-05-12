#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void initvalmat(double *mat, int n, double val, int orden);

void matmulblks(double *a, double *b, double *c, int n, int bs);

void blkmul(double *ablk, double *bblk, double *cblk, int n, int bs);

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
    int i, j, k, check = 1;
    double *a, *b, *c, *d, *e, *f, *r1, *r2, *r3;
    int n = atoi(argv[1]);
    int bs = atoi(argv[2]);
    double timetick, totalTime;

    /* Lee parametros de la linea de comando */
    if ((argc != 3) || (n <= 0) || (bs <= 0) || ((n % bs) != 0))
    {
        printf("\nParámetros inválidos. Debe utilizar %s N BS (N debe ser múltiplo de BS)\n", argv[0]);
        exit(1);
    }

    a = (double *)malloc(sizeof(double) * n * n);
    b = (double *)malloc(sizeof(double) * n * n);
    c = (double *)malloc(sizeof(double) * n * n);
    d = (double *)malloc(sizeof(double) * n * n);
    e = (double *)malloc(sizeof(double) * n * n);
    f = (double *)malloc(sizeof(double) * n * n);
    r1 = (double *)malloc(sizeof(double) * n * n);
    r2 = (double *)malloc(sizeof(double) * n * n);
    r3 = (double *)malloc(sizeof(double) * n * n);

    initvalmat(a, n, 1, 0);
    initvalmat(c, n, 1, 0);
    initvalmat(e, n, 1, 0);
    initvalmat(b, n, 1, 1);
    initvalmat(d, n, 1, 1);
    initvalmat(f, n, 1, 1);
    initvalmat(r1, n, 0, 0);
    initvalmat(r2, n, 0, 0);
    initvalmat(r3, n, 0, 0);

    timetick = dwalltime();

    matmulblks(a, b, r1, n, bs);
    matmulblks(c, d, r2, n, bs);
    matmulblks(e, f, r3, n, bs);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            r1[i * n + j] += r2[i * n + j] + r3[i * n + j];
        }
    }

    totalTime = dwalltime() - timetick;

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

    printf("Calculo de matrices (N=%d)\tTiempo total=%lf\n", n, totalTime);

    free(a);
    free(b);
    free(c);
    free(d);
    free(e);
    free(f);
    free(r1);
    free(r2);
    free(r3);
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

void matmulblks(double *a, double *b, double *c, int n, int bs)
{
    int i, j, k, offsetI, offsetJ;

    for (i = 0; i < n; i += bs)
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