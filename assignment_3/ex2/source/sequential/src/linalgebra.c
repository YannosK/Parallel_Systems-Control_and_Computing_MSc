#include "linalgebra.h"

#include <math.h>
#include <stdio.h>

// #define VERBOSE

/////////////////////////////////////////////////////////////////
// Private function definitions
/////////////////////////////////////////////////////////////////

/**
 * Return a random integer number
 * in the specified range
 * using rand().
 *
 * @param startpoint the smallest number of the range
 * @param endpoint the largest number of the range
 *
 * @returns the random number in the specified range
 */
int random_range(int startpoint, int endpoint) {

    int range = endpoint - startpoint;

    if(range > RAND_MAX)
        exit(1);

    return (rand() % (range + 1) + startpoint);
}

/////////////////////////////////////////////////////////////////
// Initialization functions
/////////////////////////////////////////////////////////////////

int random_values_vector(double *v, size_t n) {

    for(unsigned long i = 0; i < n; i++)
        v[i] = (double)random_range(-RAND_MAX / 2, RAND_MAX / 2);

    return 0;
}

int random_values_matrix(double *A, size_t m, size_t n) {
    for(size_t i = 0; i < m; i++) {
        for(size_t j = 0; j < n; j++)
            A[i * n + j] = (double)random_range(-RAND_MAX / 2, RAND_MAX / 2);
    }

    return 0;
}

/////////////////////////////////////////////////////////////////
// Operator functions
/////////////////////////////////////////////////////////////////

void vector_sum(double x[], double y[], double z[], size_t n) {
    for(size_t i = 0; i < n; i++)
        z[i] = x[i] + y[i];
}

int matrix_vector_mult(double *A, double *x, double *y, size_t m, size_t n) {
    for(size_t i = 0; i < m; i++) {
        y[i] = 0;
        for(size_t j = 0; j < n; j++)
            y[i] += A[i * n + j] * x[j];
    }

    return 0;
}

/////////////////////////////////////////////////////////////////
// Printers
/////////////////////////////////////////////////////////////////

void vector_printer(double *v, size_t n) {

    printf("\n");

    for(size_t i = 0; i < n; i++)
        printf("%.lf\n", v[i]);

    printf("\n");
}

void matrix_printer(double *A, size_t m, size_t n) {

    for(size_t i = 0; i < m; i++) {
        printf("\n|");
        for(size_t j = 0; j < n; j++)
            printf("%.0lf\t", A[i * n + j]);
        printf("\b\b\b|");
    }
    printf("\n");
}