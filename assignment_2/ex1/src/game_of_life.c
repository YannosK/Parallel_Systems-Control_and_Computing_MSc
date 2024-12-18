#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "game_of_life.h"

struct game_of_life_s {
    /*
     * The pointer to the input matrix.
     *
     * It is assumed that stores the initial state of the cells. After the
     * simulation, it will store the final state of the cells (or the input to
     * the generation that might follow).
     */
    int *input_ptr;
    /*
     * The pointer to the output matrix.
     *
     * It will be used as a buffer to store the intermediate state of the
     * cells. After the simulation, it will not store any relevant information
     * to any generations that might follow.
     */
    int *output_ptr;
    /*
     * The size of the grid (or the one dimension of the square matrix).
     */
    int grid;
};

int gol_init(game_of_life_t *const gol, const int grid) {
    if((*gol = malloc(sizeof(struct game_of_life_s))) == NULL) {
        return 1;
    };

    int *input_ptr;
    if((input_ptr = malloc(grid * grid * sizeof(int))) == NULL) {
        free(*gol);
        *gol = NULL;

        return 1;
    };

    int *output_ptr;
    if((output_ptr = malloc(grid * grid * sizeof(int))) == NULL) {
        free(input_ptr);

        free(*gol);
        *gol = NULL;

        return 1;
    }

    (*gol)->input_ptr = input_ptr;
    (*gol)->output_ptr = output_ptr;
    (*gol)->grid = grid;

    return 0;
}

int gol_destroy(game_of_life_t *const gol) {
    if(*gol == NULL) {
        return 1;
    }

    free((*gol)->input_ptr);
    free((*gol)->output_ptr);

    free(*gol);
    *gol = NULL;

    return 0;
}

int gol_parse_input_from_file(
    const game_of_life_t *const gol, const char *const filename
) {
    FILE *file = fopen(filename, "r");
    if(file == NULL) {
        return 1;
    }

    int index = 0;
    int cell;

    while(fscanf(file, "%d;", &cell) != EOF) {
        *((*gol)->input_ptr + index++) = cell;
    }

    return 0;
}

void gol_random_input(const game_of_life_t *const gol) {
    struct timeval time;
    gettimeofday(&time, NULL);
    unsigned int seed = time.tv_usec + time.tv_sec * 1000000;

    srand(seed);

    for(int i = 0; i < (*gol)->grid; i++) {
        for(int j = 0; j < (*gol)->grid; j++) {
            *((*gol)->input_ptr + (*gol)->grid * i + j) = rand() % 2;
        }
    }
}

/*
 * Print the cells of the grid.
 *
 * Parameters:
 * - arr_ptr: the matrix.
 * - size: the size of the matrix.
 */
void print_cells(const int *const arr_ptr, const int size) {
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++)
            if(*(arr_ptr + size * i + j) == 0)
                printf(" ");
            else
                printf("o");
        printf("\n");
    }
}

/*
 * Calculate the number of alive neighbors a cell has in a square matrix.
 *
 * The neighbors are assumed to be the cells that are adjacent to the cell in
 * the north, east, south, and west directions, as well as the diagonals.
 *
 * The values of the cells are either 0 or 1, where 0 represents a dead and 1 an
 * alive cell.
 *
 * Parameters:
 * - input_ptr: the input matrix.
 * - i: the row of the cell.
 * - j: the column of the cell.
 * - size: the size of the matrix.
 *
 * Returns:
 * - the number of neighbors of the cell.
 */
int gol_calculate_neighbors(
    const int *const input_ptr, const int i, const int j, const int size
) {
    int neighbors = 0;

    int north = i - 1;
    int east = j + 1;
    int south = i + 1;
    int west = j - 1;

    // Follows a branch-friendly approach to check the neighbors
    if((0 < i) && (i < size - 1) && (0 < j) && (j < size - 1)) {
        // inner cells

        // north
        neighbors += input_ptr[size * north + j];
        // north-east
        neighbors += input_ptr[size * north + east];
        // east
        neighbors += input_ptr[size * i + east];
        // south-east
        neighbors += input_ptr[size * south + east];
        // south
        neighbors += input_ptr[size * south + j];
        // south-west
        neighbors += input_ptr[size * south + west];
        // west
        neighbors += input_ptr[size * i + west];
        // north-west
        neighbors += input_ptr[size * north + west];
    } else if((i == 0) && (0 < j) && (j < size - 1)) {
        // first row - excluding corners

        // east
        neighbors += input_ptr[size * i + east];
        // south-east
        neighbors += input_ptr[size * south + east];
        // south
        neighbors += input_ptr[size * south + j];
        // south-west
        neighbors += input_ptr[size * south + west];
        // west
        neighbors += input_ptr[size * i + west];
    } else if((0 < i) && (i < size - 1) & (j == 0)) {
        // first column - excluding corners

        // north
        neighbors += input_ptr[size * north + j];
        // north-east
        neighbors += input_ptr[size * north + east];
        // east
        neighbors += input_ptr[size * i + east];
        // south-east
        neighbors += input_ptr[size * south + east];
        // south
        neighbors += input_ptr[size * south + j];
    } else if((i == size - 1) && (0 < j) && (j < size - 1)) {
        // last row - excluding corners

        // north
        neighbors += input_ptr[size * north + j];
        // north-east
        neighbors += input_ptr[size * north + east];
        // east
        neighbors += input_ptr[size * i + east];
        // west
        neighbors += input_ptr[size * i + west];
        // north-west
        neighbors += input_ptr[size * north + west];
    } else if((0 < i) && (i < size - 1) & (j == size - 1)) {
        // last column - excluding corners

        // north
        neighbors += input_ptr[size * north + j];
        // south
        neighbors += input_ptr[size * south + j];
        // south-west
        neighbors += input_ptr[size * south + west];
        // west
        neighbors += input_ptr[size * i + west];
        // north-west
        neighbors += input_ptr[size * north + west];

    } else if((i == 0) && (j == 0)) {
        // top-left corner

        // east
        neighbors += input_ptr[size * i + east];
        // south-east
        neighbors += input_ptr[size * south + east];
        // south
        neighbors += input_ptr[size * south + j];
    } else if((i == 0) && (j == size - 1)) {
        // top-right corner

        // south
        neighbors += input_ptr[size * south + j];
        // south-west
        neighbors += input_ptr[size * south + west];
        // west
        neighbors += input_ptr[size * i + west];
    } else if((i == size - 1) && (j == 0)) {
        // bottom-left corner

        // north
        neighbors += input_ptr[size * north + j];
        // north-east
        neighbors += input_ptr[size * north + east];
        // east
        neighbors += input_ptr[size * i + east];
    } else if((i == size - 1) && (j == size - 1)) {
        // bottom-right corner

        // north
        neighbors += input_ptr[size * north + j];
        // west
        neighbors += input_ptr[size * i + west];
        // north-west
        neighbors += input_ptr[size * north + west];
    }

    return neighbors;
}

/*
 * Enforce the rules of the game of life to get the state of a cell in the next
 * generation.
 *
 * The values of the cells are either 0 or 1, where 0 represents a
 * dead and 1 an alive cell.
 *
 * The rules are:
 * 1. Any live cell with fewer than two live neighbors dies, as if by
 * underpopulation.
 * 2. Any live cell with two or three live neighbors lives on to the next
 * generation.
 * 3. Any live cell with more than three live neighbors dies, as if by
 * overpopulation.
 * 4. Any dead cell with exactly three live neighbors becomes a live cell, as if
 * by reproduction.
 *
 * Parameters:
 * - input_ptr: the pointer to the input matrix.
 * - output_ptr: the pointer to the output matrix.
 * - neighbors: the number of neighbors of the cell.
 * - i: the row of the cell.
 * - j: the column of the cell.
 * - size: the size of the matrix.
 */
void gol_enforce_rules(
    const int *const input_ptr, int *const output_ptr, const int neighbors,
    const int i, const int j, const int size
) {
    if(*(input_ptr + size * i + j) == 1) {
        if(neighbors < 2) { // underpopulation
            *(output_ptr + size * i + j) = 0;
        } else if(neighbors < 4) {
            *(output_ptr + size * i + j) = 1;
        } else { // overpopulation
            *(output_ptr + size * i + j) = 0;
        }
    } else {
        if(neighbors == 3) { // reproduction
            *(output_ptr + size * i + j) = 1;
        } else {
            *(output_ptr + size * i + j) = 0;
        }
    }
}

void gol_execute(const game_of_life_t *const gol, const int generations) {
    int neighbors;
    int *temp_ptr;

#ifdef DEBUG
    printf("Initial state:\n");
    print_cells((*gol)->input_ptr, (*gol)->grid);
#endif

    for(int gen = 0; gen < generations; gen++) {
        for(int i = 0; i < (*gol)->grid; i++) {
            for(int j = 0; j < (*gol)->grid; j++) {
                neighbors = gol_calculate_neighbors(
                    (*gol)->input_ptr, i, j, (*gol)->grid
                );
                gol_enforce_rules(
                    (*gol)->input_ptr, (*gol)->output_ptr, neighbors, i, j,
                    (*gol)->grid
                );
            }
        }

#ifdef DEBUG
        printf("After generation %d:\n", gen);
        print_cells((*gol)->output_ptr, (*gol)->grid);
#endif

        temp_ptr = (*gol)->input_ptr;
        (*gol)->input_ptr = (*gol)->output_ptr;
        (*gol)->output_ptr = temp_ptr;
    }
}

void gol_execute_parallel(
    const game_of_life_t *const gol, const int generations, const int threads
) {
    int neighbors;
    int *temp_ptr;

#pragma omp parallel num_threads(threads) default(none)                        \
    shared(gol, generations, temp_ptr) private(neighbors)
    {

#ifdef DEBUG
#pragma omp single nowait
        {
            printf("Number of threads: %d\n\n", omp_get_num_threads());
            printf("Initial state:\n");
            print_cells((*gol)->input_ptr, (*gol)->grid);
        }
#endif

        for(int gen = 0; gen < generations; gen++) {
#pragma omp for schedule(static)
            for(int i = 0; i < (*gol)->grid; i++) {
                for(int j = 0; j < (*gol)->grid; j++) {
                    neighbors = gol_calculate_neighbors(
                        (*gol)->input_ptr, i, j, (*gol)->grid
                    );
                    gol_enforce_rules(
                        (*gol)->input_ptr, (*gol)->output_ptr, neighbors, i, j,
                        (*gol)->grid
                    );
                }
            }

#ifdef DEBUG
#pragma omp single
            {
                printf("After generation %d:\n", gen);
                print_cells((*gol)->output_ptr, (*gol)->grid);
            }
#endif

#pragma omp single
            {
                temp_ptr = (*gol)->input_ptr;
                (*gol)->input_ptr = (*gol)->output_ptr;
                (*gol)->output_ptr = temp_ptr;
            }
        }
    }
}
