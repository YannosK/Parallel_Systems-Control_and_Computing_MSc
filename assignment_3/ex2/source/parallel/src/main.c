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

    MPI_Datatype shared_data_mpi_t; // custom type that will contain everything
                                    // that needs to be shared

    size_t n; // number of rows and columns of matrix

    double *local_x; // local part of n x 1 vector x
    double *local_A; // local part of n x n matrix A
    double *local_y; // local part of n x 1 result vector y

    size_t local_m; // number of rows of submatrix of A, local to the process
    size_t local_n; // number of columns of submatrix of A, local to the process

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

    /***********************************
     *  Execution of the program
     ***********************************/

    /***********************************
     *  Deallocations and termination
     ***********************************/

    MPI_Finalize();

    return 0;
}

/********************************************************************************
 * Check whether any MPI process has found an error.  If so,
 * print message and terminate all processes.  Otherwise,
 * continue execution.
 *
 * @param error_code code of error from the caller
 * @param function_name name of the caller function
 * @param message message to print if there's an error
 * @param comm communicator containing processes calling this function.
 * should be `MPI_COMM_WORLD`.
 *
 * @note
 * The communicator containing the processes calling Check_for_error should be
 * `MPI_COMM_WORLD`.
 */
void error_check_mpi(
    int error_code, char function_name[], char message[], MPI_Comm comm
) {
    int ok;

    MPI_Allreduce(&error_code, &ok, 1, MPI_INT, MPI_MIN, comm);
    if(ok == 0) {
        int my_rank;
        MPI_Comm_rank(comm, &my_rank);
        if(my_rank == 0) {
            fprintf(
                stderr, "Proc %d > In %s, %s\n", my_rank, function_name, message
            );
            fflush(stderr);
        }
        MPI_Finalize();
        exit(-1);
    }
}

/********************************************************************************
 * Partition the size of the problem to local sizes
 * in order to break down the vectors and the matrix
 * to local counterparts, for parallel execution
 *
 * @param local_m pointer to variable of number of rows of local_A and elements
 * of local_x
 * @param local_n pointer to variable of number of columns of local_A and
 * elements of local_y
 * @param m number of rows of matrix A and elements of vector x
 * @param n number of columns of matrix A and elements of vector y
 * @param comm_sz the number of processes in the MPI communiator
 * @param comm communicator where the caller of this function belongs to
 *
 * @returns 0 if all run correctly.
 * @returns 2 if comm_sz does not divide the problem size exactly
 *
 * @warning
 * This function must be called at the beginning of the program,
 * after MPI is initialized and the size of the matrix and vectors is inputted.
 * It assumes that `local_m` and `local_n` have no value yet.
 * */
int problem_size_partitioning(
    size_t *local_m, size_t *local_n, size_t m, size_t n, int comm_sz,
    MPI_Comm comm
) {

    if((n <= (size_t)comm_sz) || (n % (size_t)comm_sz != 0)) {
        error_check_mpi(
            2, "problem_size_partitioning",
            "problem size too small or not divizible by process number", comm
        );
        return 2;
    }

    (*local_m) = m; // number of rows remains m in column partitioning
    (*local_n) = n / (size_t)comm_sz; // number of columns breaks down to blocks
                                      // in column partitioning

    return 0;
}

/********************************************************************************
 * Initialize local matrix and vectors:
 * local memory allocations for variabels of private scope
 * to the MPI process.
 *
 * @param local_x pointer to a local part of the vector x
 * @param local_A pointer to a local part of the matrix A
 * @param local_y pointer to a local part of the result vector y
 * @param local_m number of rows of local_A and elements of local_x
 * @param local_n number of columns of local_A and elements of local_y
 * @param my_rank rank of MPI process calling this function
 * @param comm communicator where the caller of this function belongs to
 *
 * @returns 0 if all run correctly.
 * @returns 1 if there is no memory to allocate
 *
 * @warning
 * This function must be called at the beginning of the program,
 * after MPI is initialized and the size of the matrix and vectors is inputted
 * and after the problem is partitioned to local sizes.
 * It assumes that `local_x`, `local_y` and `local_A` are just declared as
 * variables but not pointing anywhere.
 * */
int local_memory_allocations(
    double *local_x, double *local_A, double *local_y, size_t *local_m,
    size_t *local_n, size_t m, size_t n, MPI_Comm comm
) {
    local_x = (double *)calloc(sizeof(double), (*local_n));
    local_A = (double *)calloc(sizeof(double), (*local_m) * (*local_n));
    local_y = (double *)calloc(sizeof(double), (*local_m));

    if(local_x == NULL || local_y == NULL || local_A == NULL) {
        error_check_mpi(
            1, "local_memory_allocation", "Cannot allocate enough memory", comm
        );
        return 1;
    }

    return 0;
}

/********************************************************************************
 * This function initializes the custom MPI datatype
 * that is needed to send data all around.
 * More specifically it uses the entire matrix A
 * and the entire vector x, a strating point for each,
 * and `local_m` and `local_n` as offsets to create
 * a struct type that will contain:
 *
 * - a x's block of size local_n x 1
 *
 * - an A's block of size local_m x local_n
 *
 * @param mpi_datatype_p pointer to the custom MPI datatype
 * @param A pointer to the entire matrix A
 * @param x pointer to the entire vactor x
 * @param local_m number of rows of local_A and elements of local_x
 * @param local_n number of columns of local_A and elements of local_y
 * @param i_x starting index of x
 * @param i_A starting index of A
 *
 * @warning
 * - The MPI datatype must already be declared before insertion
 *
 * - the indexes of x must be a multiple of local_n
 *
 * - the indexes of A must be a multiple of local_n but not larger than n
 *
 */
void build_mpi_type(
    MPI_Datatype *mpi_datatype_p, double *A, double *x, size_t local_m,
    size_t local_n, size_t i_x, size_t i_A
) {
    // local_m arrays of local_n elements from A and one local_n sized vector
    // from x

    int array_of_block_lengths[local_m + 1];

    for(size_t i = 0; i < local_m + 1; i++)
        array_of_block_lengths[i] = local_n;
}

/********************************************************************************
 * This function creates the program data in
 * process with rank 0.
 * These data are the matrix and the vector that need to be multiplied.
 * They will have random values.
 * The data are communicated via a custom MPI data type
 * that will be created in this function.
 */
void create_and_share_program_data() {}

/********************************************************************************
 * Destroy local matrix and vectors:
 * local memory deallocations for variabels of private scope
 * to the MPI process.
 *
 * @param local_x pointer to a local part of the vector x
 * @param local_A pointer to a local part of the matrix A
 * @param local_y pointer to a local part of the result vector y
 *
 * @warning
 * This function must be called at the end of the program,
 * before calling `MPI_Finish()`.
 * All the pointers to local values will remain dangling!
 * */
void local_memory_deallocations(
    double *local_x, double *local_A, double *local_y
) {
    free(local_x);
    free(local_A);
    free(local_y);
}

// /********************************************************************************
//  * initialize matrix and vectors and send the data
//  * in process 0, and send the necessary data to other processes
//  * so that they compute the
//  *
//  * This is done in the rank 0 process
//  *
//  * @param local_x pointer to a local part of the vector x
//  * @param local_A pointer to a local part of the matrix A
//  * @param local_y pointer to a local part of the result vector y
//  * @param local_m number of rows of local_A and elements of local_x
//  * @param local_n number of columns of local_A and elements of local_y
//  * @param m number of rows of matrix A and elements of vector x
//  * @param n number of columns of matrix A and elements of vector y
//  * @param my_rank rank of MPI process calling this function
//  * @param comm communicator where the caller of this function belongs to
//  *
//  * @returns 0 if all run correctly.
//  * @returns 1 if there is no memory to allocate
//  * */
// int mpi_init_problem(
//     double *local_x, double *local_A, double *local_y, size_t local_m,
//     size_t local_n, size_t m, size_t n, int my_rank, MPI_Comm comm
// ) {
//     double *x = NULL;
//     double *y = NULL;
//     double *A = NULL;

//     if(my_rank == 0) {

//         /***********************************
//          *  Initializations
//          ***********************************/

//         x = (double *)calloc(sizeof(double), n);     // n x 1 vector
//         A = (double *)calloc(sizeof(double), m * n); // m x n matrix
//         y = (double *)calloc(sizeof(double), m);     // m x 1 result vector

//         if(x == NULL || y == NULL || A == NULL) {
//             mpi_error_check(
//                 1, "mpi_init_problem", "Cannot allocate enough memory", comm
//             );
//         }

//         random_values_vector(x, n);
//         random_values_matrix(A, m, n);

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