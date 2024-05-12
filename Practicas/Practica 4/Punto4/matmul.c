#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

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
    int i, j, k, n, check = 1;
    double *a, *b, *c;
    double timetick, totalTime;

    /* Lee parametros de la linea de comando */
    if ((argc != 2) || ((n = atoi(argv[1])) <= 0))
    {
        printf("\nUsar: %s size \n  size: Dimension de la matriz y el vector\n", argv[0]);
        exit(1);
    }
    a = (double *)malloc(sizeof(double) * n * n);
    b = (double *)malloc(sizeof(double) * n * n);
    c = (double *)malloc(sizeof(double) * n * n);

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            a[i * n + j] = 1;
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            b[i * n + j] = 1;
    
    timetick = dwalltime();

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            c[i * n + j] = 0;
            for (k = 0; k < n; k++)
            {
                c[i * n + j] += (a[i * n + k] * b[j * n + k]);
            }
        }
    }

    totalTime = dwalltime() - timetick;

    // Check results
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            check = check && (c[i * n + j] == n);

    if (check)
    {
        printf("Multiplicacion de matrices resultado correcto\n");
    }
    else
    {
        printf("Multiplicacion de matrices resultado erroneo\n");
    }

    printf("Multiplicacion de matrices (N=%d)\tTiempo total=%lf\n", n, totalTime);

    free(a);
    free(b);
    free(c);

    return 0;
}