#include <stdio.h>
#include <stdlib.h>

////////////////////////////////
// Local includes
///////////////////////////////

#include "linalgebra.h"
#include "timer.h"

////////////////////////////////
// Local defines
///////////////////////////////

#define DEBUG

////////////////////////////////
// Function Definitions
///////////////////////////////

int main(int argc, char *argv[]) {

    /***********************************
     *  Local variables
     ***********************************/

    size_t n;                      // number of rows and columns of matrix
    double start, finish, elapsed; // timing variables

    /***********************************
     *  Arguments check
     ***********************************/

    if(argc != 2) {
        printf("Usage: ./app <size>\n");
        return 1;
    }

    n = (int)strtoul(argv[1], NULL, 10);

    /***********************************
     *  Initializations & allocations
     ***********************************/

    double *x = (double *)calloc(sizeof(double), n);     // n x 1 vector
    double *A = (double *)calloc(sizeof(double), n * n); // n x n matrix
    double *y = (double *)calloc(sizeof(double), n);     // n x 1 result vector

    random_values_vector(x, n);
    random_values_matrix(A, n, n);

#ifdef DEBUG
    printf("\nVector x:\n");
    vector_printer(x, n);
    printf("\nMatrix A:\n");
    matrix_printer(A, n, n);
#endif

    /***********************************
     *  Execution of the program
     ***********************************/

    GET_TIME(start);
    matrix_vector_mult(A, x, y, n, n);
    GET_TIME(finish);

#ifdef DEBUG
    printf("\nResult vector y:\n");
    vector_printer(y, n);
#endif

    elapsed = finish - start;
    printf("Sequential matrix-vector multiplication time: %lf\n", elapsed);

    /***********************************
     *  Deallocations and termination
     ***********************************/

    free(x);
    free(A);
    free(y);

    return 0;
}