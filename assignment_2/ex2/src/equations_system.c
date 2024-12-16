#include "equations_system.h"

#include <math.h>
#include <stdio.h>

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
// Public function definitions
/////////////////////////////////////////////////////////////////

int random_values_vector(double *v, size_t n) {

    for(unsigned long i = 0; i < n; i++)
        v[i] = (double)random_range(-RAND_MAX / 2, RAND_MAX / 2);

    return 0;
}

int upper_triangular_matrix(double **A, size_t n) {

    for(long long row = (long long)n - 1; row >= 0; row--) {
        for(long long column = row; column < (long long)n; column++)
            A[row][column] = (double)random_range(-RAND_MAX / 2, RAND_MAX / 2);
    }

    return 0;
}

int back_substitution_by_row(double **A, double *b, double *x, size_t n) {

    size_t row, column;

    for(row = n - 1; row < n; row--) {
        x[row] = b[row];
        for(column = row + 1; column < n; column++)
            x[row] -= A[row][column] * x[column];
        x[row] /= A[row][row];
    }

    return 0;
}

int back_substitution_by_column(double **A, double *b, double *x, size_t n) {

    size_t row, column;

    for(row = 0; row < n; row++)
        x[row] = b[row];

    for(column = n - 1; column < n; column--) {
        x[column] /= A[column][column];
        for(row = 0; row < column; row++)
            x[row] -= A[row][column] * x[column];
    }

    return 0;
}

int back_substitution_by_row_p(
    double **A, double *b, double *x, size_t n, unsigned long threadcount
) {

    size_t row, column;

    for(row = n - 1; row < n; row--) {
        x[row] = b[row];

// might need scope specification
#pragma omp parallel for num_threads(threadcount) reduction(- : x[row])        \
    schedule(runtime)
        for(column = row + 1; column < n; column++)
            x[row] -= A[row][column] * x[column];

        x[row] /= A[row][row];
    }

    return 0;
}

int back_substitution_by_column_p(
    double **A, double *b, double *x, size_t n, unsigned long threadcount
) {

    size_t row, column;

#pragma omp parallel for num_threads(threadcount)
    for(row = 0; row < n; row++)
        x[row] = b[row];

#pragma omp parallel for schedule(runtime)
    for(column = n - 1; column < n; column--) {
        x[column] /= A[column][column];
        for(row = 0; row < column; row++)
            x[row] -= A[row][column] * x[column];
    }

    return 0;
}

/////////////////////////////////////////////////////////////////
// Debug and testers
/////////////////////////////////////////////////////////////////

double compare_to_row_method(double **A, double *b, double *x, size_t n) {

    double *y;
    y = (double *)calloc(n, sizeof(double));
    if(y == NULL)
        return 2;

    back_substitution_by_row(A, b, y, n);

    double total_difference = 0.0;
    for(size_t i = 0; i < n; i++) {
        if(y[i] != x[i])
            total_difference += fabs(y[i] - x[i]);
    }

    free(y);

    return total_difference / n;
}

double compare_to_column_method(double **A, double *b, double *x, size_t n) {

    double *y;
    y = (double *)calloc(n, sizeof(double));
    if(y == NULL)
        return -1;

    back_substitution_by_column(A, b, y, n);

    double total_difference = 0.0;
    for(size_t i = 0; i < n; i++) {
        if(y[i] != x[i])
            total_difference += fabs(y[i] - x[i]);
    }

    free(y);

    return total_difference / n;
}

/**
 * Prints a square matrix in the terminal
 *
 * @param A the name of the already initialized n x n matrix
 * @param n the number of rows and columns of the matrix
 *
 * @warning
 *
 * - it is the caller's responsibility to have the matrix initialized
 * and to also pass the correct size
 *
 * - since it prints out in the terminal, matrixes with very large
 * element values, or too many values, might not be printed correctly
 */
void matrix_printer(double **A, size_t n) {

    for(size_t i = 0; i < n; i++) {
        printf("\n|");
        for(size_t j = 0; j < n; j++)
            printf("%.0lf\t", A[i][j]);
        printf("\b\b\b|");
    }
    printf("\n");
}

void tester(size_t n) {

    for(size_t column = n - 1; column <= n - 1; column--)
        printf("%lu\n", n);

    printf("\n");

    for(long long column = (long long)n - 1; column >= 0; column--)
        printf("%lld\n", (long long)n);

    printf("\n");
}

/**
 * Prints an array of doubles in the terminal
 *
 * @param A the name of the already initialized double array
 * @param n the number of elements in the array
 *
 * @warning
 *
 * - it is the caller's responsibility to have the vector initialized
 * and to also pass the correct size
 *
 * - it prints up to fourth decimal digit accuracy
 */
void vector_printer(double *v, size_t n) {

    printf("\n");

    for(size_t i = 0; i < n; i++)
        printf("%.17lf\n", v[i]);

    printf("\n");
}
