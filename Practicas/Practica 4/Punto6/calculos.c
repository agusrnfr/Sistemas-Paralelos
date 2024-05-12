#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <limits.h>

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
    int i, n;
    double *a;
    double timetick, totalTime;
    int sum = 0, min = INT_MAX, max = INT_MIN;
    double prom;

    if ((argc != 2) || ((n = atoi(argv[1])) <= 0))
    {
        printf("\nUsar: %s size \n  size: Tamaño del vector\n", argv[0]);
        exit(1);
    }
    a = (double *)malloc(sizeof(double) * n);

    for (i = 0; i < n; i++)
        a[i] = i;

    timetick = dwalltime();

    for (i = 0; i < n; i++)
    {

        if (a[i] < min)
        {
            min = a[i];
        }

        if (a[i] > max)
        {
            max = a[i];
        }

        sum += a[i];
    }

    prom = sum / n;

    totalTime = dwalltime() - timetick;

    printf("Tiempo en segundos %f\n", totalTime);

    printf("Minimo: %d\n", min);
    printf("Maximo: %d\n", max);
    printf("Promedio: %f\n", prom);

    // CHECK

    if ((max == n - 1) && (min == 0) && (prom == (n - 1) / 2))
    {
        printf("Resultado correcto\n");
    }
    else
    {
        printf("Resultado incorrecto\n");
    }
}