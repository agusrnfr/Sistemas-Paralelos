// Ejercicio 2
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
	double x, partial, scale;
	int i;
	int N = atoi(argv[1]);
	scale = 2.78;
	x = 0.0;

    for (i = 1; i <= N; i++)
    {
		x = x + sqrt(i * scale) + 2 * x;
    }

	printf("\n Resultado: %f \n", x);

	return (0);
}
