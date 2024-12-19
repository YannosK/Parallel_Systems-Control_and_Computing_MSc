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

    double start, finish, elapsed_seq, elapsed_par; // timing variables
    double rel_er; // variable to store relative errors

    unsigned long thread_count = 1; // number of threads
    size_t n = 1;                   // problem size

    double *x;  // solution x vector
    double *b;  // zero order coefficients
    double **A; // coefficient table

    /***********************************
     *  Argument check
     ***********************************/

    if(argc != 5) {
        printf("Usage: ./main <threadsnum> <problemsize> <rows-or-columns> "
               "<parallel-or-sequential>\n");
        return 1;
    }

    thread_count = strtoul(argv[1], NULL, 10);
    n = strtoul(argv[2], NULL, 10);
    const char *method = argv[3];
    const char *execution = argv[4];

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
     *  Execution of the program
     ***********************************/

    if(!strcmp(execution, "sequential")) {

        if(!strcmp(method, "rows")) {

            GET_TIME(start);
            back_substitution_by_row(A, b, x, n);
            GET_TIME(finish);
            elapsed_seq = finish - start;
            printf(
                "\nSequential row method execution time: %lf\n", elapsed_seq
            );

            rel_er = compare_to_column_method(A, b, x, n);

            printf("\nRelative error to column method: %.7lf\n", rel_er);

        } else if(!strcmp(method, "columns")) {

            GET_TIME(start);
            back_substitution_by_column(A, b, x, n);
            GET_TIME(finish);
            elapsed_seq = finish - start;
            printf(
                "\nSequential column method execution time: %lf\n", elapsed_seq
            );

            rel_er = compare_to_row_method(A, b, x, n);

            printf("\nRelative error to row method: %.7lf\n", rel_er);

        } else {
            printf("Method input does not match any known method\n");
            exit(3);
        }
    } else if(!strcmp(execution, "parallel")) {

        if(!strcmp(method, "rows")) {

            GET_TIME(start);
            back_substitution_by_row_p(A, b, x, n, thread_count);
            GET_TIME(finish);
            elapsed_par = finish - start;
            printf("\nParallel row method execution time: %lf\n", elapsed_par);

            GET_TIME(start);
            rel_er = compare_to_row_method(A, b, x, n);
            GET_TIME(finish);
            elapsed_seq = finish - start;
            printf("Sequential row method execution time: %lf\n", elapsed_seq);
            printf("Speedup: %lf\n", elapsed_seq / elapsed_par);

            printf("\nRelative error to sequential method: %.lf\n", rel_er);

        } else if(!strcmp(method, "columns")) {

            GET_TIME(start);
            back_substitution_by_column_p(A, b, x, n, thread_count);
            GET_TIME(finish);
            elapsed_par = finish - start;
            printf(
                "\nParallel column method execution time: %lf\n", elapsed_par
            );

            GET_TIME(start);
            rel_er = compare_to_column_method(A, b, x, n);
            GET_TIME(finish);
            elapsed_seq = finish - start;
            printf(
                "Sequential column method execution time: %lf\n", elapsed_seq
            );
            printf("Speedup: %lf\n", elapsed_seq / elapsed_par);

            printf("\nRelative error to sequential method: %.lf\n", rel_er);

        } else {
            printf("Method input does not match any known method\n");
            exit(3);
        }
    } else {
        printf(
            "Execution input does not match parallel or sequential execution\n"
        );
        exit(3);
    }

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
