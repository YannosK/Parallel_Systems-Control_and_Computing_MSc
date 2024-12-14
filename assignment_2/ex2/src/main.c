// #include <omp.h>
#include <stdio.h>
#include <stdlib.h>

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

unsigned long thread_count = 1; // number of threads
unsigned long i = 0;            // iterator index
int ret;                        // return value holder variable

int main(int argc, char *argv[]) {

    /***********************************
     *  Local variables
     ***********************************/

    double start, finish, elapsed; // timing variables

    size_t n = 1; // problem size

    double *x;  // solution x vector
    double *b;  // zero order coefficients
    double **A; // coefficient table

    /***********************************
     *  Argument check
     ***********************************/

    if(argc != 3) {
        printf("Usage: ./main <threadsnum> <problemsize>\n");
        return 1;
    }

    thread_count = strtoul(argv[1], NULL, 10);
    n = strtoul(argv[2], NULL, 10);

    /***********************************
     *  Memory allocations
     ***********************************/

    x = (double *)malloc(n * sizeof(double));
    if(x == NULL)
        return 2;

    b = (double *)malloc(n * sizeof(double));
    if(b == NULL)
        return 2;

    A = (double **)malloc(n * sizeof(double *));
    if(A == NULL)
        return 2;
    for(unsigned long i = 0; i < n; i++) {
        A[i] = (double *)malloc(n * sizeof(double));
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

    /***********************************
     *  Pragmas and Parallel Execution
     ***********************************/

    GET_TIME(start);

    // #pragma omp parallel for num_threads(thread_count) reduction(+ : sum)          \
//     schedule(runtime                                                           \
//     ) // Specify by exporting the proper OMP_SCHEDULE argument in the CLI
    //     for(i = 0; i < n; i++)
    //         sum += f(i);

    GET_TIME(finish);

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
