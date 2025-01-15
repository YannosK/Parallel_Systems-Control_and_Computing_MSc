#ifndef __GAME_OF_LIFE_H__
#define __GAME_OF_LIFE_H__

#include <mpi.h>

typedef struct game_of_life_s *game_of_life_t;

/*
 * Initialize the game of life object.
 *
 * Parameters:
 * - gol: the game of life object.
 * - grid_rows: the number of rows of the grid.
 * - grid_cols: the number of columns of the grid.
 *
 * Returns:
 * - 0 if the function executed successfully.
 * - 1 if there an error occurred.
 */
int gol_init(
    game_of_life_t *const gol, const int grid_rows, const int grid_cols
);

/*
 * Destroy the game of life object.
 *
 * Parameters:
 * - gol: the game of life object.
 *
 * Returns:
 * - 0 if the function executed successfully.
 * - 1 if there an error occurred.
 */
int gol_destroy(game_of_life_t *const gol);

/*
 * Parse input from file. Used only for debugging purposes.
 *
 * It is assumed that the input file contains a square matrix of integers
 * separated by semicolons. The value 1 represents an alive cell, and the
 * value 0 represents a dead cell. The matrix is stored in row-major order.
 *
 * Also, the gol object should be able to store all the
 * values of the input file that correspond to it,
 * as there is no check for overflows.
 *
 * Parameters:
 * - gol: the game of life object.
 * - filename: the name of the file to read from.
 * - grid: the total size of the square array.
 * - comm_sz: the number of processes.
 * - comm_rank: the rank of the current process.
 * - comm: the MPI communicator.
 *
 * Returns:
 * - 0 if the function executed successfully.
 * - 1 if there an error occurred.
 */
int gol_parse_input_from_file(
    const game_of_life_t *const gol, const char *const filename, const int grid,
    const int comm_sz, const int comm_rank, const MPI_Comm comm
);

/*
 * Generate random input for the game of life object.
 *
 * It is guaranteed that the input matrix will be filled with different values
 * each time the function is called (in the pseudorandom limits).
 *
 * Parameters:
 * - gol: the game of life object.
 * - grid: the size of the grid.
 * - comm_sz: the number of processes.
 * - comm_rank: the rank of the current process.
 * - comm: the MPI communicator.
 *
 * Returns:
 * - 0 if the function executed successfully.
 * - 1 if there an error occurred.
 */
int gol_random_input(
    const game_of_life_t *const gol, const int grid, const int comm_sz,
    const int comm_rank, const MPI_Comm comm
);

/*
 * Execute the game of life for a given number of generations.
 *
 * Parameters:
 * - gol: the game of life object.
 * - comm_sz: the number of processes.
 * - comm_rank: the rank of the current process.
 * - comm: the MPI communicator.
 * - generations: the number of generations to simulate.
 */
void gol_execute(
    const game_of_life_t *const gol, const int comm_sz, const int comm_rank,
    const MPI_Comm comm, const int generations
);

/*
 * Print the game of life grid after gathering the results
 * from all the processes.
 *
 * Parameters:
 * - gol: the game of life object.
 * - grid: the size of the grid.
 * - comm_sz: the number of processes.
 * - comm_rank: the rank of the current process.
 * - comm: the MPI communicator.
 *
 * Returns:
 * - 0 if the function executed successfully.
 * - 1 if there an error occurred.
 */
int gol_print(
    const game_of_life_t *const gol, const int grid, const int comm_sz,
    const int comm_rank, const MPI_Comm comm
);

#endif
