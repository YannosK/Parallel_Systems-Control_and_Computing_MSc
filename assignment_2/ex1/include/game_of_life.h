#ifndef __GAME_OF_LIFE_H__
#define __GAME_OF_LIFE_H__

typedef struct game_of_life_s *game_of_life_t;

/*
 * Initialize the game of life object.
 *
 * Parameters:
 * - gol: the game of life object.
 * - grid: the size of the grid.
 *
 * Returns:
 * - 0 if the function executed successfully.
 * - 1 if there an error occurred.
 */
int gol_init(game_of_life_t *const gol, const int grid);

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

 * It is assumed that the input file contains a square matrix of integers
 * separated by semicolons. The value 1 represents an alive cell, and the
 * value 0 represents a dead cell. The matrix is stored in row-major order.
 *
 * Also, the gol object should be able to store all the
 * values of the input file as there is no check for overflows.
 *
 * Parameters:
 * - gol: the game of life object.
 * - filename: the name of the file to read from.
 *
 * Returns:
 * - 0 if the function executed successfully.
 * - 1 if there an error occurred.
 */
int gol_parse_input_from_file(
    const game_of_life_t *const gol, const char *const filename
);

/*
 * Generate random input for the game of life object.
 *
 * It is guaranteed that the input matrix will be filled with different values
 * each time the function is called (in the pseudorandom limits).
 *
 * Parameters:
 * - gol: the game of life object.
 */
void gol_random_input(const game_of_life_t *const gol);

/*
 * Execute the game of life for a given number of generations.
 *
 * Parameters:
 * - gol: the game of life object.
 * - generations: the number of generations to simulate.
 */
void gol_execute(const game_of_life_t *const gol, const int generations);

/*
 * Execute the game of life for a given number of generations in parallel.
 *
 * Parameters:
 * - gol: the game of life object.
 * - generations: the number of generations to simulate.
 * - threads: the number of threads to use.
 */
void gol_execute_parallel(
    const game_of_life_t *const gol, const int generations, const int threads
);

#endif
