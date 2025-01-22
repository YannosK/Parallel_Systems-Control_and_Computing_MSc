#ifndef _LINALGEBRA_H_
#define _LINALGEBRA_H_

#include <stdlib.h>

/////////////////////////////////////////////////////////////////
// Initialization functions
/////////////////////////////////////////////////////////////////

/**
 * This function fills in an
 * already initialized array of doubles
 * with random values using the `rand()` function
 * in the range [-RAND_MAX/2, RAND_MAX/2].
 * This function has sequential execution and
 * it is regarded as an initialization function.
 *
 * @param v the name of the aolready initialized array
 * @param n the number of elements it contains
 *
 * @return 0 if all executed correctly
 *
 * @warning it is the caller's responsibility to have the vector initialized
 * and to also pass the correct size
 */
int random_values_vector(double *v, size_t n);

/**
 * A function to fill in an m x n C-style matrix
 * with random elements.
 * The elements are produced by the `rand()` function.
 * They are in the range [-RAND_MAX/2, RAND_MAX/2].
 * and even though they are integers they are cast as doubles.
 * This function has sequential execution and
 * it is regarded as an initialization function.
 *
 * @param A the name of the already initialized m x n matrix
 * @param m the number of rows of the matrix
 * @param n the number of columns of the matrix
 *
 * @return 0 if all executed correctly
 *
 * @warning
 * - the matrix must be initialized as a 1D array
 *
 * - it is the caller's responsibility to have the matrix initialized
 * and to also pass the correct size
 */
int random_values_matrix(double *A, size_t m, size_t n);

/////////////////////////////////////////////////////////////////
// Operator functions
/////////////////////////////////////////////////////////////////

/**
 * Implements addition of two vectors of the same size
 * and storage to a third one of the same size.
 * The function has sequential execution.
 *
 * @param x first operand of addition
 * @param y second operand of addition
 * @param z result vector of addition
 * @param n number of elements of the vectors
 *
 * @warning it is the caller's responsibility to have the vector initialized
 * and to also pass the correct size
 */
void vector_sum(double x[], double y[], double z[], size_t n);

/**
 * Multiply a C-style m x n matrix A with a n x 1 vector x
 * to produce a m x 1 vector y: A x = y.
 *
 * The function has a sequential execution.
 *
 * @param A the name of the already initialized, as a 2D array, matrix
 * @param x the name of the already initialized vector x
 * @param y the name of the already initialized vector y
 * @param m the number of rows of the matrix and elements of the y vector
 * @param n the number of columns of the matrix and elements of the x vector
 *
 * @returns 0 if all run as ecpected
 *
 * @warning
 * - it is the caller's responsibility to have the vectors and the matrix
 * initialized and to also pass the correct size
 *
 * - the matrix must be initialized as a 1D array
 *
 * - previous elements of y will be lost
 */
int matrix_vector_mult(double *A, double *x, double *y, size_t m, size_t n);

/////////////////////////////////////////////////////////////////
// Printers
/////////////////////////////////////////////////////////////////

/**
 * Prints an array of doubles in the terminal
 *
 * @param A the name of the already initialized double array
 * @param n the number of elements in the array
 *
 * @warning it is the caller's responsibility to have the vector initialized
 * and to also pass the correct size
 */
void vector_printer(double *v, size_t n);

/**
 * Prints C-style matrix  of doubles in the terminal.
 *
 * @param A the name of the already initialized m x n matrix
 * @param m the number of rows of the matrix
 * @param n the number of columns of the matrix
 *
 * @warning
 *
 * - this matrix must be initialized as a 1D array
 *
 * - it is the caller's responsibility to have the matrix initialized
 * and to also pass the correct size
 *
 * - since it prints out in the terminal, matrixes with very large
 * element values, or too many values, might not be printed correctly
 */
void matrix_printer(double *A, size_t m, size_t n);

#endif