#include <stdio.h>
#include <stdlib.h>
#include "my_rand.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./main <shotsnum>\n");
        return 1;
    }

    long long shotsnum = strtoll(argv[1], NULL, 10);
    long long circle_shots = 0;
    double x, y;

    for (int i = 0; i < shotsnum; i++)
    {
        if ((x * x + y * y) <= 1.)
            circle_shots++;
    }

    double pi = (4 * circle_shots) / ((double)shotsnum);

    printf("Pi estimation: %lf\n", pi);

    return 0;
}