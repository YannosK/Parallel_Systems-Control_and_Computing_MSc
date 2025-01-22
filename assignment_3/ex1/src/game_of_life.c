#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#ifdef DEBUG
#include <unistd.h>
#define WAIT_US 200000
#endif

#include "error.h"
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
     * The size of the total grid, splitted among the processes, plus the
     * borders.
     */
    int grid;
    /*
     * The grid rows plus the borders.
     */
    int grid_rows;
    /*
     * The grid columns plus the borders.
     */
    int grid_cols;
    /*
     * The number of processes the grid is split into.
     */
    int comm_sz;
    /*
     * The rank of the current process.
     */
    int comm_rank;
    /*
     * The MPI communicator used to communicate between processes.
     */
    MPI_Comm comm;
};

int gol_init(
    game_of_life_t *const gol, const int grid, const int grid_rows,
    const int grid_cols, const int comm_sz, const int comm_rank,
    const MPI_Comm comm
) {
    if((*gol = malloc(sizeof(struct game_of_life_s))) == NULL) {
        return 1;
    };

    int *input_ptr;
    if((input_ptr = calloc((grid_rows + 2) * (grid_cols + 2), sizeof(int))) ==
       NULL) {
        free(*gol);
        *gol = NULL;

        return 1;
    };

    int *output_ptr;
    if((output_ptr = calloc((grid_rows + 2) * (grid_cols + 2), sizeof(int))) ==
       NULL) {
        free(input_ptr);

        free(*gol);
        *gol = NULL;

        return 1;
    }

    (*gol)->input_ptr = input_ptr;
    (*gol)->output_ptr = output_ptr;
    (*gol)->grid = grid + 2;
    (*gol)->grid_rows = grid_rows + 2;
    (*gol)->grid_cols = grid_cols + 2;
    (*gol)->comm_sz = comm_sz;
    (*gol)->comm_rank = comm_rank;
    (*gol)->comm = comm;

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

/*
 * Print the cells of the grid.
 *
 * The alive cells are drawn as 'o', the dead cells are drawn as ' '
 * and the horizontal and vertical border cells are drawn as '-' and '|',
 * respectively.
 *
 * Parameters:
 * - arr_ptr: the matrix.
 * - rows: the number of rows.
 * - cols: the number of columns.
 * - show_borders: whether to show the horizontal borders or not.
 */
void print_cells(
    const int *const arr_ptr, const int rows, const int cols,
    const int show_borders
) {
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++)
            if((i == 0) || (i == rows - 1)) {
                if(show_borders) {
                    printf("-"); // horizontal border
                } else if((j == 0) || (j == cols - 1)) {
                    printf("-"); // horizontal border
                } else {
                    if(*(arr_ptr + cols * i + j) == 0)
                        printf(" "); // dead cell
                    else
                        printf("o"); // alive cell
                }
            } else if((j == 0) || (j == cols - 1)) {
                printf("|"); // vertical border
            } else if(*(arr_ptr + cols * i + j) == 0)
                printf(" "); // dead cell
            else
                printf("o"); // alive cell
        printf("\n");
    }
}

/*
 * Parse input from file into an array.
 *
 * 1. No check about the size of the array is performed.
 * 2. The array is assumed to be initialized with zeros.
 * 3. The first and last rows and columns are not filled,
 *    since they are treated as borders. The values on the
 *    borders will remain as the array was initialized.
 *
 * Parameters:
 * - filename: the name of the file to read from.
 * - input_ptr: the pointer to the array.
 * - grid: the total size of the square array.
 *
 * Returns:
 * - 0 if the function executed successfully.
 * - 1 if there an error occurred.
 */
int parse_input_from_file_to_array(
    const char *const filename, int *const input_ptr, const int grid
) {
    FILE *file = fopen(filename, "r");
    if(file == NULL) {
        return 1;
    }

    int index = grid - 1;
    int cell;

    while(fscanf(file, "%d;", &cell) != EOF) {
        // reached east border
        if((index + 1) % (grid) == 0) {
            index += 2; // skip west border
        }
        *(input_ptr + index++) = cell;
    }

    return 0;
}

int gol_parse_input_from_file(
    const game_of_life_t *const gol, const char *const filename
) {
    if((*gol)->comm_rank == 0) {
        int *input_ptr;
        if((input_ptr = calloc((*gol)->grid * (*gol)->grid, sizeof(int))) ==
           NULL) {
            return 1;
        };

        if(parse_input_from_file_to_array(filename, input_ptr, (*gol)->grid) !=
           0) {
            return 1;
        }

#ifdef DEBUG
        print_cells(input_ptr, (*gol)->grid, (*gol)->grid, 1);
#endif

        memcpy(
            (*gol)->input_ptr, input_ptr,
            (*gol)->grid_rows * (*gol)->grid_cols * sizeof(int)
        );

#ifdef DEBUG
        print_cells((*gol)->input_ptr, (*gol)->grid_rows, (*gol)->grid_cols, 0);
#endif

        for(int i = 1; i < (*gol)->comm_sz; i++) {
            MPI_Send(
                input_ptr + i * ((*gol)->grid_rows - 2) * (*gol)->grid_cols,
                (*gol)->grid_rows * (*gol)->grid_cols, MPI_INT, i, 0,
                (*gol)->comm
            );
        }

        free(input_ptr);
    } else {
        MPI_Recv(
            (*gol)->input_ptr, (*gol)->grid_rows * (*gol)->grid_cols, MPI_INT,
            0, 0, (*gol)->comm, MPI_STATUS_IGNORE
        );

#ifdef DEBUG
        sleep((*gol)->comm_rank);
        print_cells((*gol)->input_ptr, (*gol)->grid_rows, (*gol)->grid_cols, 0);
#endif
    }

    return 0;
}

/*
 * Fill the input array with random values in {0, 1}.
 * The borders of the array are not filled and remain as is.
 *
 * Parameters:
 * - input_ptr: the pointer to the array.
 * - grid: the total size of the square array.
 */
void random_input(int *const input_ptr, const int grid) {
    struct timeval time;
    gettimeofday(&time, NULL);
    unsigned int seed = time.tv_usec + time.tv_sec * 1000000;

    srand(seed);

    for(int i = 1; i < grid - 1; i++) {
        for(int j = 1; j < grid - 1; j++) {
            *(input_ptr + grid * i + j) = rand() % 2;
        }
    }
}

int gol_random_input(const game_of_life_t *const gol) {
    if((*gol)->comm_rank == 0) {
        int *input_ptr;
        if((input_ptr = calloc((*gol)->grid * (*gol)->grid, sizeof(int))) ==
           NULL) {
            return 1;
        };

        random_input(input_ptr, (*gol)->grid);

#ifdef DEBUG
        print_cells(input_ptr, (*gol)->grid, (*gol)->grid, 1);
#endif

        memcpy(
            (*gol)->input_ptr, input_ptr,
            (*gol)->grid_rows * (*gol)->grid_cols * sizeof(int)
        );

#ifdef DEBUG
        print_cells((*gol)->input_ptr, (*gol)->grid_rows, (*gol)->grid_cols, 0);
#endif

        for(int i = 1; i < (*gol)->comm_sz; i++) {
            MPI_Send(
                input_ptr + i * ((*gol)->grid_rows - 2) * (*gol)->grid_cols,
                (*gol)->grid_rows * (*gol)->grid_cols, MPI_INT, i, 0,
                (*gol)->comm
            );
        }

        free(input_ptr);
    } else {
        MPI_Recv(
            (*gol)->input_ptr, (*gol)->grid_rows * (*gol)->grid_cols, MPI_INT,
            0, 0, (*gol)->comm, MPI_STATUS_IGNORE
        );

#ifdef DEBUG
        sleep((*gol)->comm_rank);
        print_cells((*gol)->input_ptr, (*gol)->grid_rows, (*gol)->grid_cols, 0);
#endif
    }

    return 0;
}

/*
 * Calculate the number of neighbors of a cell.
 *
 * Parameters:
 * - gol: the game of life object.
 * - i: the row of the cell.
 * - j: the column of the cell.
 *
 * Returns:
 * - the number of neighbors of the cell.
 */
int calculate_neighbors(
    const game_of_life_t *const gol, const int i, const int j
) {
    return (*gol)->input_ptr[(*gol)->grid_cols * (i - 1) + j] +
           (*gol)->input_ptr[(*gol)->grid_cols * (i - 1) + (j + 1)] +
           (*gol)->input_ptr[(*gol)->grid_cols * i + (j + 1)] +
           (*gol)->input_ptr[(*gol)->grid_cols * (i + 1) + (j + 1)] +
           (*gol)->input_ptr[(*gol)->grid_cols * (i + 1) + j] +
           (*gol)->input_ptr[(*gol)->grid_cols * (i + 1) + (j - 1)] +
           (*gol)->input_ptr[(*gol)->grid_cols * i + (j - 1)] +
           (*gol)->input_ptr[(*gol)->grid_cols * (i - 1) + (j - 1)];
}

/*
 * Enforce the rules of the game of life. Those are:
 *
 * 1. Any live cell with fewer than two live neighbors dies, as if by
 * underpopulation.
 * 2. Any live cell with two or three live neighbors lives on to the next
 * generation.
 * 3. Any live cell with more than three live neighbors dies, as if by
 * overpopulation.
 * 4. Any dead cell with exactly three live neighbors becomes a live cell, as
 * if by reproduction.
 *
 * Take from: https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
 *
 * Parameters:
 * - gol: the game of life object.
 * - i: the row of the cell.
 * - j: the column of the cell.
 * - neighbors: the number of neighbors of the cell.
 */
void enforce_rules(
    const game_of_life_t *const gol, const int i, const int j,
    const int neighbors
) {
    // Alive cell
    if((*gol)->input_ptr[(*gol)->grid_cols * i + j] == 1) {
        if(neighbors < 2) { // underpopulation
            (*gol)->output_ptr[(*gol)->grid_cols * i + j] = 0;
        } else if(neighbors < 4) {
            (*gol)->output_ptr[(*gol)->grid_cols * i + j] = 1;
        } else { // overpopulation
            (*gol)->output_ptr[(*gol)->grid_cols * i + j] = 0;
        }
    } else {
        if(neighbors == 3) { // reproduction
            (*gol)->output_ptr[(*gol)->grid_cols * i + j] = 1;
        } else {
            (*gol)->output_ptr[(*gol)->grid_cols * i + j] = 0;
        }
    }
}

/*
 * Calculate the ranks for the north and south processes with with
 * to exchange information.
 *
 * Parameters:
 * - comm_rank: the rank of the current process.
 * - comm_rank_north: the rank of the north process.
 * - comm_rank_south: the rank of the south process.
 * - comm_sz: the number of processes.
 *
 * Returns:
 * - the ranks for the north and south processes. If the process is
 * not valid then the rank is set to MPI_PROC_NULL.
 */
void exchange_neighbors(
    const int comm_rank, int *const comm_rank_north, int *const comm_rank_south,
    const int comm_sz
) {
    if(comm_rank - 1 < 0) {
        *comm_rank_north = MPI_PROC_NULL;
    } else {
        *comm_rank_north = comm_rank - 1;
    }

    if(comm_rank + 1 >= comm_sz) {
        *comm_rank_south = MPI_PROC_NULL;
    } else {
        *comm_rank_south = comm_rank + 1;
    }
}

/*
 * Exchange information with the north process.
 *
 * If the north process is not valid, then the
 * function returns immediately.
 *
 * On the other hand, the sending process sends
 * the first non-border line of its grid to the
 * north process and stores the receiving line
 * from the north process in the first line
 * of its grid (the border).
 *
 * Parameters:
 * - gol: the game of life object.
 * - comm_rank_north: the rank of the north process.
 * - comm: the MPI communicator.
 */
void exchange_north(
    const game_of_life_t *const gol, const int comm_rank_north,
    const MPI_Comm comm
) {
    if(comm_rank_north == MPI_PROC_NULL) {
        return;
    }

    const void *sendbuf = (*gol)->input_ptr + (*gol)->grid_cols;
    int sendcount = (*gol)->grid_cols;
    void *recvbuf = (*gol)->input_ptr;
    int recvcount = (*gol)->grid_cols;

    // #ifdef DEBUG
    //     sleep(comm_rank_north);
    //     printf("----------- North -----------\n");
    //     print_cells(sendbuf, 1, sendcount, 0);
    //     fflush(stdout);
    // #endif

    MPI_Sendrecv(
        sendbuf, sendcount, MPI_INT, comm_rank_north, 0, recvbuf, recvcount,
        MPI_INT, comm_rank_north, 0, comm, MPI_STATUS_IGNORE
    );

    // #ifdef DEBUG
    //     print_cells(recvbuf, 1, recvcount, 0);
    //     fflush(stdout);
    // #endif
}

/*
 * Exchange information with the south process.
 *
 * If the south process is not valid, then the
 * function returns immediately.
 *
 * On the other hand, the sending process sends
 * the last non-border line of its grid to the
 * south process and stores the receiving line
 * from the south process in the last line
 * of its grid (the border).
 *
 * Parameters:
 * - gol: the game of life object.
 * - comm_rank_south: the rank of the south process.
 * - comm: the MPI communicator.
 */
void exchange_south(
    const game_of_life_t *const gol, const int comm_rank_south,
    const MPI_Comm comm
) {
    if(comm_rank_south == MPI_PROC_NULL) {
        return;
    }

    const void *sendbuf =
        (*gol)->input_ptr + (*gol)->grid_cols * ((*gol)->grid_rows - 2);
    int sendcount = (*gol)->grid_cols;
    void *recvbuf =
        (*gol)->input_ptr + ((*gol)->grid_cols) * ((*gol)->grid_rows - 1);
    int recvcount = (*gol)->grid_cols;

    // #ifdef DEBUG
    //     sleep(comm_rank_south);
    //     printf("----------- South -----------\n");
    //     print_cells(sendbuf, 1, sendcount, 0);
    //     fflush(stdout);
    // #endif

    MPI_Sendrecv(
        sendbuf, sendcount, MPI_INT, comm_rank_south, 0, recvbuf, recvcount,
        MPI_INT, comm_rank_south, 0, comm, MPI_STATUS_IGNORE
    );

    // #ifdef DEBUG
    //     print_cells(recvbuf, 1, recvcount, 0);
    //     fflush(stdout);
    // #endif
}

void gol_execute(const game_of_life_t *const gol, const int generations) {
    int neighbors;
    int *temp_ptr;
    int comm_rank_north, comm_rank_south;

#ifdef DEBUG
    MPI_Barrier((*gol)->comm);
    sleep((*gol)->comm_rank);
    if((*gol)->comm_rank == 0) {
        printf("Initial Configuration:\n");
    }
    print_cells((*gol)->input_ptr, (*gol)->grid_rows, (*gol)->grid_cols, 1);
#endif

    for(int gen = 0; gen < generations; gen++) {
        exchange_neighbors(
            (*gol)->comm_rank, &comm_rank_north, &comm_rank_south,
            (*gol)->comm_sz
        );

        exchange_north(gol, comm_rank_north, (*gol)->comm);
        exchange_south(gol, comm_rank_south, (*gol)->comm);

        // Iterate over non-border cells
        for(int i = 1; i < (*gol)->grid_rows - 1; i++) {
            for(int j = 1; j < (*gol)->grid_cols - 1; j++) {
                // Calculate the number of neighbors
                neighbors = calculate_neighbors(gol, i, j);
                // Enforce the rules of the game of life
                enforce_rules(gol, i, j, neighbors);
            }
        }

        temp_ptr = (*gol)->input_ptr;
        (*gol)->input_ptr = (*gol)->output_ptr;
        (*gol)->output_ptr = temp_ptr;

#ifdef DEBUG
        MPI_Barrier((*gol)->comm);
        sleep((*gol)->comm_rank);
        if((*gol)->comm_rank == 0) {
            printf("After generation %d:\n", gen);
        }
        print_cells((*gol)->input_ptr, (*gol)->grid_rows, (*gol)->grid_cols, 1);
#endif

        MPI_Barrier((*gol)->comm);
    }
}

int gol_print(const game_of_life_t *const gol) {
    int local_grid_elements = (*gol)->grid_cols * ((*gol)->grid_rows - 2);

    if((*gol)->comm_rank == 0) {
        int *input_ptr;
        if((input_ptr = calloc((*gol)->grid * (*gol)->grid, sizeof(int))) ==
           NULL) {
            return 1;
        };

        memcpy(
            input_ptr, (*gol)->input_ptr,
            ((*gol)->grid_rows - 1) * (*gol)->grid_cols * sizeof(int)
        );

        int step = (*gol)->grid_cols * ((*gol)->grid_rows - 1);

        for(int i = 1; i < (*gol)->comm_sz; i++) {
            MPI_Recv(
                input_ptr + i * step, local_grid_elements, MPI_INT, i, 0,
                (*gol)->comm, MPI_STATUS_IGNORE
            );
        }

        print_cells(input_ptr, (*gol)->grid, (*gol)->grid, 1);

        free(input_ptr);
    } else {

        MPI_Send(
            (*gol)->input_ptr + (*gol)->grid_cols, local_grid_elements, MPI_INT,
            0, 0, (*gol)->comm
        );
    }

    return 0;
}
