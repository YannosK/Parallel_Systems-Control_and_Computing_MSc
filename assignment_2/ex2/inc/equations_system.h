#ifndef _EQUATIONS_SYSTEM_H_
#define _EQUATIONS_SYSTEM_H_

#include <omp.h>
#include <stdlib.h>

/**
 * This function fills in an
 * already initialized array of doubles
 * with random values using the `rand()` function
 * in the range [-255, 255].
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
 * A function to fill in a n x n matrix
 * with random elements in an upper triangular form.
 * The elements are produced by the `rand()` function.
 * They are in the range [-255, 255].
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

/**
 * Back-substitution on the linear equation system with the computation executed
 * row by row. The equation system can be represented as A x = b x is a vector
 * containing the unknowns where A is an n x n upper triangular matrix with the
 * coefficients of the x's and b is a vector of the zero order coefficients.
 *
 * This is a sequential execution algorithm.
 *
 * @param A the upper triangular n x n matrix of the coefficients
 * @param b the zero order coefficient vector
 * @param x vector of unkown x values
 * @param n the size of the linear equations system
 *
 * @returns 0 if executed correctly. This means that after execution x will
 * contain the computed values of the unknowns.
 *
 * @warning it is the caller's responsibility to have the matrix and the
 * vector's initialized and to also pass the correct size
 */
int back_substitution_by_row(double **A, double *b, double *x, size_t n);

/**
 * Back-substitution on the linear equation system with the computation executed
 * column by column. The equation system can be represented as A x = b x is a
 * vector containing the unknowns where A is an n x n upper triangular matrix
 * with the coefficients of the x's and b is a vector of the zero order
 * coefficients.
 *
 * This is a sequential execution algorithm.
 *
 * @param A the upper triangular n x n matrix of the coefficients
 * @param b the zero order coefficient vector
 * @param x vector of unkown x values
 * @param n the size of the linear equations system
 *
 * @returns 0 if executed correctly. This means that after execution x will
 * contain the computed values of the unknowns.
 *
 * @warning it is the caller's responsibility to have the matrix and the
 * vector's initialized and to also pass the correct size
 */
int back_substitution_by_column(double **A, double *b, double *x, size_t n);

/**
 * @attention this is the multi-threaded version of `back_substitution_by_row(A,
 * b, x, n)` using OpenMP.
 *
 * Back-substitution on the linear equation system with the computation executed
 * row by row. The equation system can be represented as A x = b x is a vector
 * containing the unknowns where A is an n x n upper triangular matrix with the
 * coefficients of the x's and b is a vector of the zero order coefficients.
 *
 * @param A the upper triangular n x n matrix of the coefficients
 * @param b the zero order coefficient vector
 * @param x vector of unkown x values
 * @param n the size of the linear equations system
 * @param threadcount the number of threads of execution
 *
 * @returns 0 if executed correctly. This means that after execution x will
 * contain the computed values of the unknowns.
 *
 * @warning it is the caller's responsibility to have the matrix and the
 * vector's initialized and to also pass the correct size
 */
int back_substitution_by_row_p(
    double **A, double *b, double *x, size_t n, unsigned long threadcount
);

/**
 * @attention this is the multi-threaded version of
 * `back_substitution_by_column(A, b, x, n)` using OpenMP.
 *
 * Back-substitution on the linear equation system with the computation executed
 * column by column. The equation system can be represented as A x = b x is a
 * vector containing the unknowns where A is an n x n upper triangular matrix
 * with the coefficients of the x's and b is a vector of the zero order
 * coefficients.
 *
 * @param A the upper triangular n x n matrix of the coefficients
 * @param b the zero order coefficient vector
 * @param x vector of unkown x values
 * @param n the size of the linear equations system
 * @param threadcount the number of threads of execution
 *
 * @returns 0 if executed correctly. This means that after execution x will
 * contain the computed values of the unknowns.
 *
 * @warning it is the caller's responsibility to have the matrix and the
 * vector's initialized and to also pass the correct size
 */
int back_substitution_by_column_p(
    double **A, double *b, double *x, size_t n, unsigned long threadcount
);

/**
 * Compares resutls to the ones produced if `back_substitution_by_row(A, b,
 * x, n)`.
 *
 * @param A the upper triangular n x n matrix of the coefficients
 * @param b the zero order coefficient vector
 * @param x vector of unkown x values that contain the results
 * @param n the size of the linear equations system
 *
 * @returns their absolute difference. It returns -1 for failure.
 *
 * @warning it is the caller's responsibility to have the matrix and the
 * vector's initialized and to also pass the correct size.
 */
double compare_to_row_method(double **A, double *b, double *x, size_t n);

/**
 * Compares resutls to the ones produced if `back_substitution_by_column(A, b,
 * x, n)`.
 *
 * @param A the upper triangular n x n matrix of the coefficients
 * @param b the zero order coefficient vector
 * @param x vector of unkown x values that contain the results
 * @param n the size of the linear equations system
 *
 * @returns their absolute difference. It returns -1 for failure.
 *
 * @warning it is the caller's responsibility to have the matrix and the
 * vector's initialized and to also pass the correct size.
 */
double compare_to_column_method(double **A, double *b, double *x, size_t n);

#endif