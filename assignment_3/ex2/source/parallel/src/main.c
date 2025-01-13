#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

////////////////////////////////
// Local includes
///////////////////////////////

#include "linalgebra.h"

////////////////////////////////
// Local defines
///////////////////////////////

////////////////////////////////
// Global Variables
///////////////////////////////

////////////////////////////////
// Function Declerations
///////////////////////////////

void mpi_error_check(int local_ok, char fname[], char message[], MPI_Comm comm);
// void mpi_init_problem(
//     double local_A[], double local_x[], size_t local_m, size_t local_n,
//     size_t m, size_t n, int my_rank, MPI_Comm comm
// );
// void mpi_matrix_vector_mul(
//     double local_A[], double local_x[], double local_y[], size_t local_m,
//     size_t local_n, size_t m, int my_rank, MPI_Comm comm
// );

////////////////////////////////
// Function Definitions
///////////////////////////////

int main(int argc, char *argv[]) {

    /***********************************
     *  Local variables
     ***********************************/

    int comm_sz;   // number of processes
    int my_rank;   // rank of current process
    MPI_Comm comm; // MPI communicator for the program

    size_t n; // number of rows and columns of matrix

    /***********************************
     *  Arguments check
     ***********************************/

    if(argc != 2) {
        printf("Usage: mpiexec -n <processes> ./app <size>\n");
        return 1;
    }

    n = (int)strtoul(argv[1], NULL, 10);

    /***********************************
     *  Initializations & allocations
     ***********************************/

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;

    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);

    if(comm_sz >= (int)n) {
        if(my_rank == 0)
            fprintf(stderr, "Too small problem size and too many processes\n");
        exit(2);
    } else if((int)n % comm_sz != 0) {
        if(my_rank == 0)
            fprintf(
                stderr,
                "Problem size must be divisible by the number of processes\n"
            );
        exit(3);
    }

    //     local_m = m / comm_sz; // this order is important or comm_sz might
    //     not have
    //                            // been initialized
    //     local_n = n; // not saying so explicitly anywhere, but I just guessed

    //     local_x = (double *)calloc(sizeof(double), local_n);
    //     local_y = (double *)calloc(sizeof(double), local_m);

    //     local_A = (double *)calloc(sizeof(double), local_m * local_n);

    /***********************************
     *  Execution of the program
     ***********************************/

    //     mpi_init_problem(
    //         local_A, local_x, local_m, local_n, m, n, my_rank, MPI_COMM_WORLD
    //     );

    //     mpi_matrix_vector_mul(
    //         local_A, local_x, local_y, local_m, local_n, m, my_rank,
    //         MPI_COMM_WORLD
    //     );

    // #ifdef VERBOSE
    //     printf("\n\nRank %d\n", my_rank);
    //     printf("matrix:\n");
    //     cmatrix_printer(local_A, local_m, local_n);
    //     printf("vector x:\n");
    //     vector_printer(local_x, local_n);
    //     printf("local y result is:");
    //     vector_printer(local_y, local_m);
    // #endif

    /***********************************
     *  Deallocations and termination
     ***********************************/

    //     free(local_x);
    //     free(local_y);

    //     free(local_A);

    MPI_Finalize();

    return 0;
}

////////////////////////////////
// Function Definitions
///////////////////////////////

/********************************************************************************
 * Check whether any process has found an error.  If so,
 * print message and terminate all processes.  Otherwise,
 * continue execution.
 *
 * @param local_ok 1 if calling process has found an error, 0 otherwise
 * @param fname name of function calling `mpi_error_check()`
 * @param message message to print if there's an error
 * @param comm communicator containing processes calling this function.
 * should be `MPI_COMM_WORLD`.
 *
 * @note
 * The communicator containing the processes calling Check_for_error should be
 * `MPI_COMM_WORLD`.
 */
void mpi_error_check(
    int local_ok /* in */, char fname[] /* in */, char message[] /* in */,
    MPI_Comm comm /* in */
) {
    int ok;

    MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
    if(ok == 0) {
        int my_rank;
        MPI_Comm_rank(comm, &my_rank);
        if(my_rank == 0) {
            fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname, message);
            fflush(stderr);
        }
        MPI_Finalize();
        exit(-1);
    }
}

// /********************************************************************************
//  * initialize matrix and vectors and send the data.
//  *
//  * This is done in the rank 0 process
//  * */
// void mpi_init_problem(
//     double local_A[], double local_x[], size_t local_m, size_t local_n,
//     size_t m, size_t n, int my_rank, MPI_Comm comm
// ) {
//     double *x = NULL;
//     double *y = NULL;
//     double *A = NULL;

//     if(my_rank == 0) {

//         // perform initializations
//         x = (double *)calloc(sizeof(double), n);
//         random_values_vector(x, n);

//         y = (double *)calloc(sizeof(double), m);
//         random_values_vector(y, m);

//         A = (double *)calloc(sizeof(double), m * n);
//         random_values_cmatrix(A, m, n);

//         // scatter chunks of A to all the processes
//         MPI_Scatter(
//             A, local_m * local_n, MPI_DOUBLE, local_A, local_m * local_n,
//             MPI_DOUBLE, 0, comm
//         );

//         // broadcast x to everyone - allgather does not work here
//         for(size_t k = 0; k < local_n; k++)
//             local_x[k] = x[k];
//         MPI_Bcast(local_x, local_n, MPI_DOUBLE, 0, comm);

//         // deallocations
//         free(x);
//         free(y);
//         free(A);
//     } else {
//         // scatter chunks of A to all the processes - A might be NULL I think
//         MPI_Scatter(
//             A, local_m * local_n, MPI_DOUBLE, local_A, local_m * local_n,
//             MPI_DOUBLE, 0, comm
//         );
//         // receiving end of broadcast
//         MPI_Bcast(local_x, local_n, MPI_DOUBLE, 0, comm);
//     }
// }

// /********************************************************************************
//  *
//  * */
// void mpi_matrix_vector_mul(
//     double local_A[], double local_x[], double local_y[], size_t local_m,
//     size_t local_n, size_t m, int my_rank, MPI_Comm comm
// ) {
//     double *result = (double *)calloc(
//         sizeof(double), m
//     ); // local variable for result storage

//     cmatrix_vector_mult(local_A, local_x, local_y, local_m, local_n);

// #ifdef ALLGATHER_RESULT
//     MPI_Allgather(
//         local_y, local_m, MPI_DOUBLE, result, local_m, MPI_DOUBLE, comm
//     );
//     printf("\nresult as gatherd in process %d:\n", my_rank);
//     vector_printer(result, m);
// #else
//     (void)my_rank;
// #endif

//     free(result);
// }