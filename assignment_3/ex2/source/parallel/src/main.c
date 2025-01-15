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

// #define DEBUG
// #define VERBOSE

////////////////////////////////
// Function Declerations
///////////////////////////////

int problem_size_partitioning(
    size_t *local_m, size_t *local_n, size_t m, size_t n, int comm_sz,
    MPI_Comm comm
);

int local_memory_allocations(
    double **recv_buffer, double **result_buffer, double **y, size_t local_m,
    size_t local_n, size_t m, int my_rank, MPI_Comm comm
);

int create_and_share_program_data(
    size_t m, size_t n, size_t local_m, size_t local_n, double **recv_buffer,
    double *timing_start, int comm_sz, int my_rank, MPI_Comm comm
);

void calculate_local_mult(
    size_t local_m, size_t local_n, double *recv_buffer, double **result_buffer
);

void reduce_to_final_result(
    double *result_buffer, double **y, size_t m, double *timing_end,
    int my_rank, MPI_Comm comm
);

void local_memory_deallocations(
    double **recv_buffer, double **result_buffer, double **y
);

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

    double *recv_buffer = NULL;   // receive buffer for data in custom MPI type
    double *result_buffer = NULL; // buffer for local results
    double *y = NULL;             // final result vector

    size_t local_m; // number of rows of submatrix of A, local to the process
    size_t local_n; // number of columns of submatrix of A, local to the process

    double local_start = 0.0, local_finish = 0.0, local_elapsed, elapsed = 0.0;

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

    if(comm_sz > (int)n) {
        if(my_rank == 0)
            fprintf(
                stderr,
                "Too small problem size (%lu) and too many processes (%d)\n", n,
                comm_sz
            );
        exit(2);
    } else if((int)n % comm_sz != 0) {
        if(my_rank == 0)
            fprintf(
                stderr,
                "Problem size (%lu) must be divisible by the number of "
                "processes (%d)\n",
                n, comm_sz
            );
        exit(3);
    }

    /***********************************
     *  Execution of the program
     ***********************************/

    problem_size_partitioning(&local_m, &local_n, n, n, comm_sz, comm);

    local_memory_allocations(
        &recv_buffer, &result_buffer, &y, local_m, local_n, n, my_rank, comm
    );

#ifdef DEBUG
    if(my_rank == 0) {
        printf(
            "\nlocal_m: %lu\nlocal_n: %lu\ncomm_sz: %d", local_m, local_n,
            comm_sz
        );
    }
#endif

    create_and_share_program_data(
        n, n, local_m, local_n, &recv_buffer, &local_start, comm_sz, my_rank,
        comm
    );

    calculate_local_mult(local_m, local_n, recv_buffer, &result_buffer);

    reduce_to_final_result(result_buffer, &y, n, &local_finish, my_rank, comm);

    /***********************************
     *  Timing of program
     ***********************************/

    local_elapsed = local_finish - local_start;
    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

    if(my_rank == 0) {

#ifdef VERBOSE
        printf("\nResult vector:\n");
        vector_printer(y, n);
#endif
        printf("Parallel matrix-vector multiplication time: %lf\n", elapsed);
    }

    /***********************************
     *  Deallocations and termination
     ***********************************/

    local_memory_deallocations(&recv_buffer, &result_buffer, &y);

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

    MPI_Allreduce(&error_code, &ok, 1, MPI_INT, MPI_MAX, comm);
    if(ok != 0) {
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
 * of local_y
 * @param local_n pointer to variable of number of columns of local_A and
 * elements of local_x
 * @param m number of rows of matrix A and elements of vector y
 * @param n number of columns of matrix A and elements of vector x
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

    if((n < (size_t)comm_sz) || (n % (size_t)comm_sz != 0)) {
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
 * @param recv_buffer pointer to the local buffer that receives the intial data
 * @param result_buffer pointer to a local part of the result vector y
 * @param y pointer to the final result buffer
 * @param local_m number of rows of local_A and elements of local_y
 * @param local_n number of columns of local_A and elements of local_x
 * @param m number of rows of matrix A and elements of vector y
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
    double **recv_buffer, double **result_buffer, double **y, size_t local_m,
    size_t local_n, size_t m, int my_rank, MPI_Comm comm
) {
    /***********************************
     * Receive buffer
     * - receives custom MPI datatype
     * - Contains IN THAT ORDER:
     *  + submatrix of local_m x local_n
     *  + subvector of local_n x 1
     ***********************************/

    size_t recv_buffer_len = local_m * local_n + local_n;
    double *recv_buffer_p;

    recv_buffer_p = (double *)calloc(sizeof(double), recv_buffer_len);

    if(recv_buffer_p == NULL) {
        error_check_mpi(
            1, "local_memory_allocation", "Cannot allocate enough memory", comm
        );
        return 1;
    }

    (*recv_buffer) = recv_buffer_p;

    /***********************************
     * Result buffer
     * - local partial results of y
     * - vector of local_m x 1
     ***********************************/

    (*result_buffer) = (double *)calloc(sizeof(double), local_m);

    if((*result_buffer) == NULL) {
        error_check_mpi(
            1, "local_memory_allocation", "Cannot allocate enough memory", comm
        );
        return 1;
    }

    if(my_rank == 0) {

        /***********************************
         * Final result buffer
         * - resides in process 0
         * - vector y
         * - size m x 1
         ***********************************/

        (*y) = (double *)calloc(sizeof(double), m);

        if((*y) == NULL) {
            error_check_mpi(
                1, "local_memory_allocation", "Cannot allocate enough memory",
                comm
            );
            return 1;
        }
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
 * @param local_m number of rows of local_A and elements of local_y
 * @param local_n number of columns of local_A and elements of local_x
 * @param n the number of columns of matrix A
 * @param i_x starting index of x
 * @param i_A starting index of A
 * @param my_rank rank of MPI process calling this function
 * @param comm communicator where the caller of this function belongs to
 *
 * @returns 0 if correct
 * @returns 1 if the indexes of x is wrong
 * @returns 2 if the index of A is wrong
 *
 * @warning
 * - The MPI datatype must already be declared before insertion
 *
 * - the indexes of x must be a multiple of local_n
 *
 * - the indexes of A must be a multiple of local_n but not larger than n -
 * local_n
 *
 */
int build_mpi_type(
    MPI_Datatype *mpi_datatype_p, double *A, double *x, size_t local_m,
    size_t local_n, size_t n, size_t i_x, size_t i_A, int my_rank, MPI_Comm comm
) {
    /***********************************
     * Indexes check
     ***********************************/

    if(i_x % local_n != 0) {
        error_check_mpi(
            1, "build_mpi_type",
            "the index of vector x is not in the expected range", comm
        );
        return 1;
    } else if((i_A % local_n != 0) || (i_A > n - local_n)) {
        error_check_mpi(
            2, "build_mpi_type",
            "the index of matrix A is not in the expected range", comm
        );
        return 2;
    }

    /***********************************
     * Count of elements
     * @warning
     * I think one array specifies
     * as one element
     ***********************************/

    int count = local_m + 1;

    /***********************************
     * Specifying block lengths
     * - each contains local_n
     *  + there are local_m rows
     *  + there is local_x
     ***********************************/

    int array_of_block_lengths[local_m + 1];
    for(size_t i = 0; i < local_m + 1; i++)
        array_of_block_lengths[i] = local_n;

    /***********************************
     * Specifying displacements
     * - run through submatrix of A
     *  + the elements will be lcoal_A
     * - in the end add local_x
     * - prev, next to get differences
     ***********************************/

    MPI_Aint array_of_displacements[local_m + 1];

    if(my_rank == 0) {
        MPI_Aint base_addr, next_addr;

        MPI_Get_address(A, &base_addr);
        for(size_t i = 0; i < local_m; i++) {
            MPI_Get_address((A + i_A + i * n), &next_addr);
            array_of_displacements[i] = next_addr - base_addr;
        }

        MPI_Get_address((x + i_x), &next_addr);
        array_of_displacements[local_m] = next_addr - base_addr;

        // #ifdef DEBUG
        //         printf("\nArray of displacements has the following
        //         values\n"); printf("For A index %lu and x index %lu\n",
        //         i_A, i_x); for(size_t i = 0; i < local_m; i++) {
        //             printf("%ld\n", array_of_displacements[i]);
        //         }
        // #endif

    } else {
        for(size_t i = 0; i < local_m + 1; i++) {
            array_of_displacements[i] = i * local_n * sizeof(double);
        }
    }

    /***********************************
     * Specifying the datatypes
     * - all are just double
     ***********************************/

    MPI_Datatype array_of_types[local_m + 1];
    for(size_t i = 0; i < local_m + 1; i++)
        array_of_types[i] = MPI_DOUBLE;

    /***********************************
     * Create the type
     ***********************************/

    MPI_Type_create_struct(
        count, array_of_block_lengths, array_of_displacements, array_of_types,
        mpi_datatype_p
    );

    MPI_Type_commit(mpi_datatype_p);

    return 0;
}

/********************************************************************************
 * This function creates the program data in
 * process with rank 0.
 * These data are the matrix and the vector that need to be multiplied.
 * They will have random values.
 * The data are communicated via a custom MPI data types,
 * that packages in a struct local_m array elements
 * that constitute the partitioned by columns submatrixes of A
 * of size local_m x local_n
 * and parts of the vector x of local_n elements.
 * The goal is to create the initial matrix A and vector x
 * and send the custom datatype to each process
 *
 * This function uses datatypes and then deallocates them.
 * It initializes them as an array and uses `MPI_Send`
 * and `MPI_Recv` to send them to other processes
 *
 * @param m number of rows of matrix A and elements of result vector y
 * @param n number of columns of matrix A and elements of vector x
 * @param local_m number of rows of local_A and elements of local_y
 * @param local_n number of columns of local_A and elements of local_x
 * @param recv_buffer pointer to local buffer that receives the intial data
 * @param timing_start a pointer to a local varibale used to snapshot the
 * execution start
 * @param comm_sz the number of processes in the communicator
 * @param my_rank rank of MPI process calling this function
 * @param comm communicator where the caller of this function belongs to
 *
 * @returns 0 if all run correctly.
 * @returns 1 if memory allocations failed.
 *
 * @warning
 *
 * - Call this function after problem size partitioning, local memory
 * allocations and MPI type commit
 *
 * - it is the caller's responsibility to insert correct sizes
 *
 */
int create_and_share_program_data(
    size_t m, size_t n, size_t local_m, size_t local_n, double **recv_buffer,
    double *timing_start, int comm_sz, int my_rank, MPI_Comm comm
) {
    double *A = NULL;
    double *x = NULL;

    MPI_Datatype array_of_datatypes[comm_sz];

    if(my_rank == 0) {

        /***********************************
         * Allocations of originals
         * - matrix A of m x n
         * - vector x of n x 1
         * - fill with random values
         ***********************************/

        A = (double *)calloc(sizeof(double), m * n);
        x = (double *)calloc(sizeof(double), n);

        if(A == NULL || x == NULL) {
            fprintf(stderr, "Memory allocation failed");
            fflush(stderr);
        }

        random_values_vector(x, n);
        random_values_matrix(A, m, n);

#ifdef DEBUG
        printf("\nVector x:\n");
        vector_printer(x, n);
        printf("\nMatrix A:\n");
        matrix_printer(A, m, n);
#endif
    }

    /***********************************
     * Custom types
     * - breaking A into submatrixes
     * - breaking x into subvectors
     ***********************************/

    for(int i = 0; i < comm_sz; i++)
        build_mpi_type(
            (array_of_datatypes + i), A, x, local_m, local_n, n, local_n * i,
            local_n * i, my_rank, comm
        );

    /***********************************
     * Start timing the program
     ***********************************/
    MPI_Barrier(comm);
    (*timing_start) = MPI_Wtime();

    if(my_rank == 0) {

        /***********************************
         * Scatter the data to the processes
         * - Starting point is always A
         *  + types handle internally offsets
         * - for every process
         *  + use a corresponding type from array
         *  + no need to send to yourself,
         *    but do the allocation manually
         ***********************************/

        // keep local data
        for(size_t i = 0; i < local_m; i++) {
            for(size_t j = 0; j < local_n; j++)
                *((*recv_buffer) + i * local_n + j) = A[i * n + j];
        }
        for(size_t k = 0; k < local_n; k++)
            *((*recv_buffer) + local_m * local_n + k) = x[k];

        // send data to others
        for(int rank = 1; rank < comm_sz; rank++)
            MPI_Send(A, 1, array_of_datatypes[rank], rank, 0, comm);

        /***********************************
         * free initial matrix and vector
         ***********************************/

        free(A);
        free(x);

    } else {

        /***********************************
         * Receive custom type
         * - Sent from process 0
         * - automatically stored to local buff
         ***********************************/
        MPI_Recv(
            (*recv_buffer), 1, array_of_datatypes[my_rank], 0, 0, comm,
            MPI_STATUS_IGNORE
        );
    }

    /***********************************
     * free custom types
     ***********************************/

    for(int i = 0; i < comm_sz; i++)
        MPI_Type_free(array_of_datatypes + i);

    return 0;
}

/********************************************************************************
 * Calculate multiplications of local submatrixes and subvectors
 * and store results locally.
 * The results are vectors of size local_m x 1.
 * The subvectors and submatrixes are contained in
 * the `recv_buffer` of each process and the results
 * will be stored in the `result_buffers` of the processes.
 *
 * @param local_m number of rows of local_A and elements of local_y
 * @param local_n number of columns of local_A and elements of local_x
 * @param recv_buffer pointer to local buffer that receives the intial data
 * @param result_buffer pointer to a local part of the result vector y
 *
 * @warning
 * - Call this function after caling the `create_and_share_program_data`
 * function and all the previous calls it needs
 *
 * - The user is responsible to input correct sizes
 * */
void calculate_local_mult(
    size_t local_m, size_t local_n, double *recv_buffer, double **result_buffer
) {

    matrix_vector_mult(
        recv_buffer, recv_buffer + local_m * local_n, *result_buffer, local_m,
        local_n
    );
}

/********************************************************************************
 * This function performs a vector sum of all the local result vectors,
 * reducing them to a single vector in process 0.
 * This process will contain the final result and it could print it,
 * or discard it.
 * Effectively this function ends the program.
 *
 * @param result_buffer pointer to a local part of the result vector y
 * @param y pointer to the final result buffer
 * @param m number of rows of matrix A and elements of result vector y
 * @param timing_end a pointer to a local varibale used to snapshot the
 * execution end
 * @param my_rank rank of MPI process calling this function
 * @param comm communicator where the caller of this function belongs to
 *
 * @warning
 * - Call this function after caling the `calculate_local_mult`
 * function and all the previous calls it needs
 */
void reduce_to_final_result(
    double *result_buffer, double **y, size_t m, double *timing_end,
    int my_rank, MPI_Comm comm
) {
    if(my_rank == 0) {

        MPI_Reduce(result_buffer, (*y), m, MPI_DOUBLE, MPI_SUM, 0, comm);

    } else {
        MPI_Reduce(result_buffer, (*y), m, MPI_DOUBLE, MPI_SUM, 0, comm);
    }

    (*timing_end) = MPI_Wtime();
}

/********************************************************************************
 * Destroy local matrix and vectors:
 * local memory deallocations for variabels of private scope
 * to the MPI process.
 *
 * @param recv_buffer pointer to local buffer that receives the intial data
 * @param result_buffer pointer to a local part of the result vector y
 * @param y pointer to the final result buffer
 *
 * @warning
 * This function must be called at the end of the program,
 * before calling `MPI_Finish()`.
 * All the pointers to local values will remain dangling!
 * */
void local_memory_deallocations(
    double **recv_buffer, double **result_buffer, double **y
) {
    free(*recv_buffer);
    free(*result_buffer);
    free(*y);
}