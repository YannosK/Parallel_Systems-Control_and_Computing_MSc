#include <stdio.h>
#include <stdlib.h>

#include "serial.h"
#include "timer.h"

int serial(unsigned long long int throws, double *pi, double *time) {
    unsigned int seed = 0;

    double start, end;

    GET_TIME(start);

    unsigned long long int throw;
    unsigned long long int throws_in_circle = 0;

    double x, y, square_distance;

    for(throw = 0; throw < throws; throw ++) {
        x = 2.0 * rand_r(&seed) / RAND_MAX - 1;
        y = 2.0 * rand_r(&seed) / RAND_MAX - 1;

        square_distance = x * x + y * y;

        if(square_distance <= 1) {
            throws_in_circle++;
        }
    }

    *pi = 4 * (double)throws_in_circle / throws;

    GET_TIME(end);

    *time = end - start;

    return 0;
}