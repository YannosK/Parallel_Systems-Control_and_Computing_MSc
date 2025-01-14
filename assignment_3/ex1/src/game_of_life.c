#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#ifdef DEBUG
#include <unistd.h>
#define WAIT_US 200000
#endif

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
     * The edge size of the grid plus the borders.
     */
    int grid;
};

int gol_init(game_of_life_t *const gol, const int grid) {
    if((*gol = malloc(sizeof(struct game_of_life_s))) == NULL) {
        return 1;
    };

    int *input_ptr;
    if((input_ptr = calloc((grid + 2) * (grid + 2), sizeof(int))) == NULL) {
        free(*gol);
        *gol = NULL;

        return 1;
    };

    int *output_ptr;
    if((output_ptr = calloc((grid + 2) * (grid + 2), sizeof(int))) == NULL) {
        free(input_ptr);

        free(*gol);
        *gol = NULL;

        return 1;
    }

    (*gol)->input_ptr = input_ptr;
    (*gol)->output_ptr = output_ptr;
    (*gol)->grid = grid + 2;

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

    int index = (*gol)->grid - 1;
    int cell;

    while(fscanf(file, "%d;", &cell) != EOF) {
        // reached east border
        if(index % ((*gol)->grid - 1) == 0) {
            index += 2; // skip west border
        }
        *((*gol)->input_ptr + index++) = cell;
    }

    return 0;
}

void gol_random_input(const game_of_life_t *const gol) {
    struct timeval time;
    gettimeofday(&time, NULL);
    unsigned int seed = time.tv_usec + time.tv_sec * 1000000;

    srand(seed);

    for(int i = 1; i < (*gol)->grid - 1; i++) {
        for(int j = 1; j < (*gol)->grid - 1; j++) {
            *((*gol)->input_ptr + (*gol)->grid * i + j) = rand() % 2;
        }
    }
}

/*
 * Print the cells of the grid.
 *
 * The alive cells are drawn as 'o', the dead cells are drawn as ' '
 * and the horizontal and vertical border cells are drawn as '-' and '|',
 * respectively.
 *
 * Parameters:
 * - arr_ptr: the matrix.
 * - size: the size of the matrix.
 */
void print_cells(const int *const arr_ptr, const int size) {
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++)
            if((i == 0) || (i == size - 1)) {
                printf("-"); // horizontal border
            } else if((j == 0) || (j == size - 1)) {
                printf("|"); // vertical border
            } else if(*(arr_ptr + size * i + j) == 0)
                printf(" "); // dead cell
            else
                printf("o"); // alive cell
        printf("\n");
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
        for(int i = 1; i < (*gol)->grid - 1; i++) {
            for(int j = 1; j < (*gol)->grid - 1; j++) {
                // Calculate the number of neighbors
                neighbors =
                    (*gol)->input_ptr[(*gol)->grid * (i - 1) + j] +
                    (*gol)->input_ptr[(*gol)->grid * (i - 1) + (j + 1)] +
                    (*gol)->input_ptr[(*gol)->grid * i + (j + 1)] +
                    (*gol)->input_ptr[(*gol)->grid * (i + 1) + (j + 1)] +
                    (*gol)->input_ptr[(*gol)->grid * (i + 1) + j] +
                    (*gol)->input_ptr[(*gol)->grid * (i + 1) + (j - 1)] +
                    (*gol)->input_ptr[(*gol)->grid * i + (j - 1)] +
                    (*gol)->input_ptr[(*gol)->grid * (i - 1) + (j - 1)];

                // Enforce the rules of the game of life
                if((*gol)->input_ptr[(*gol)->grid * i + j] == 1) {
                    if(neighbors < 2) { // underpopulation
                        (*gol)->output_ptr[(*gol)->grid * i + j] = 0;
                    } else if(neighbors < 4) {
                        (*gol)->output_ptr[(*gol)->grid * i + j] = 1;
                    } else { // overpopulation
                        (*gol)->output_ptr[(*gol)->grid * i + j] = 0;
                    }
                } else {
                    if(neighbors == 3) { // reproduction
                        (*gol)->output_ptr[(*gol)->grid * i + j] = 1;
                    } else {
                        (*gol)->output_ptr[(*gol)->grid * i + j] = 0;
                    }
                }
            }
        }

#ifdef DEBUG
        usleep(WAIT_US);
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
            for(int i = 1; i < (*gol)->grid - 1; i++) {
                for(int j = 1; j < (*gol)->grid - 1; j++) {
                    // Calculate the number of neighbors
                    neighbors =
                        (*gol)->input_ptr[(*gol)->grid * (i - 1) + j] +
                        (*gol)->input_ptr[(*gol)->grid * (i - 1) + (j + 1)] +
                        (*gol)->input_ptr[(*gol)->grid * i + (j + 1)] +
                        (*gol)->input_ptr[(*gol)->grid * (i + 1) + (j + 1)] +
                        (*gol)->input_ptr[(*gol)->grid * (i + 1) + j] +
                        (*gol)->input_ptr[(*gol)->grid * (i + 1) + (j - 1)] +
                        (*gol)->input_ptr[(*gol)->grid * i + (j - 1)] +
                        (*gol)->input_ptr[(*gol)->grid * (i - 1) + (j - 1)];

                    // Enforce the rules of the game of life
                    if((*gol)->input_ptr[(*gol)->grid * i + j] == 1) {
                        if(neighbors < 2) { // underpopulation
                            (*gol)->output_ptr[(*gol)->grid * i + j] = 0;
                        } else if(neighbors < 4) {
                            (*gol)->output_ptr[(*gol)->grid * i + j] = 1;
                        } else { // overpopulation
                            (*gol)->output_ptr[(*gol)->grid * i + j] = 0;
                        }
                    } else {
                        if(neighbors == 3) { // reproduction
                            (*gol)->output_ptr[(*gol)->grid * i + j] = 1;
                        } else {
                            (*gol)->output_ptr[(*gol)->grid * i + j] = 0;
                        }
                    }
                }
            }

#ifdef DEBUG
#pragma omp single
            {
                usleep(WAIT_US);
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
