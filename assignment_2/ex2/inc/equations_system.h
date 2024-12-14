#ifndef _EQUATIONS_SYSTEM_H_
#define _EQUATIONS_SYSTEM_H_

/**
 * This function fills in an
 * already initialized array of doubles
 * with random values using the `rand()` function with seed = 1.
 * This function has sequential execution and
 * it is regarded as an initialization function.
 *
 * @param v the name of the aolready initialized array
 * @param n the number of elements it contains
 *
 * @warning it is the caller's responsibility to have the vector initialized
 * and to also pass the correct size
 */
int random_values_vector(double *v, size_t n);

/**
 * A function to fill in a n x n matrix
 * with random elements in an upper triangular form.
 * The elements are produced by the `rand()` function with seed = 1
 * and even they are integers they are cast as doubles.
 * This function has sequential execution and
 * it is regarded as an initialization function.
 *
 * @param A the name of the already initialized n x n matrix
 * @param n the number of rows and columns of the matrix
 *
 * @return 0 if all executed correctly
 *
 * @warning it is the caller's responsibility to have the matrix initialized
 * and to also pass the correct size
 */
int upper_triangular_matrix(double **A, size_t n);

#endif