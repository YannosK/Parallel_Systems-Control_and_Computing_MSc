// #include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

////////////////////////////////
// Local includes
///////////////////////////////

#include "equations_system.h"
#include "timer.h"

////////////////////////////////
// Private defines
///////////////////////////////

////////////////////////////////
// Global Variables
///////////////////////////////

unsigned long i = 0; // iterator index
int ret;             // return value holder variable

int main(int argc, char *argv[]) {

    /***********************************
     *  Local variables
     ***********************************/

    double start, finish, elapsed; // timing variables

    unsigned long thread_count = 1; // number of threads
    size_t n = 1;                   // problem size

    double *x;  // solution x vector
    double *b;  // zero order coefficients
    double **A; // coefficient table

    /***********************************
     *  Argument check
     ***********************************/

    if(argc != 4) {
        printf("Usage: ./main <threadsnum> <problemsize> <method>\n");
        return 1;
    }

    thread_count = strtoul(argv[1], NULL, 10);
    n = strtoul(argv[2], NULL, 10);
    const char *method = argv[3];

    /***********************************
     *  Memory allocations
     ***********************************/

    x = (double *)calloc(n, sizeof(double));
    if(x == NULL)
        return 2;

    b = (double *)calloc(n, sizeof(double));
    if(b == NULL)
        return 2;

    A = (double **)calloc(n, sizeof(double *));
    if(A == NULL)
        return 2;
    for(unsigned long i = 0; i < n; i++) {
        A[i] = (double *)calloc(n, sizeof(double));
        if(A[i] == NULL)
            return 2;
    }

    /***********************************
     *  Initializations
     ***********************************/

    ret = random_values_vector(b, n);
    if(ret)
        exit(ret);

    ret = upper_triangular_matrix(A, n);
    if(ret)
        exit(ret);

    srand(2);

    /***********************************
     *  Pragmas and Parallel Execution
     ***********************************/

    if(!strcmp(method, "rows")) {

        GET_TIME(start);
        back_substitution_by_row(A, b, x, n);
        GET_TIME(finish);

        printf(
            "Average difference from column back-substitution method: %.7lf\n",
            compare_to_column_method(A, b, x, n)
        );

    } else if(!strcmp(method, "columns")) {

        GET_TIME(start);
        back_substitution_by_column(A, b, x, n);
        GET_TIME(finish);

        printf(
            "Average difference from row back-substitution method: %.7lf\n",
            compare_to_row_method(A, b, x, n)
        );

    } else {
        printf("Method input does not match any known method\n");
        exit(3);
    }

    /***********************************
     *  Final calculations
     ***********************************/

    elapsed = finish - start;
    printf("\nElapsed time: %lf\n", elapsed);

    /***********************************
     *  Delete, destroy and deallocation
     ***********************************/

    free(x);
    free(b);
    for(unsigned long i = 0; i < n; i++)
        free(A[i]);
    free(A);

    return 0;
}
