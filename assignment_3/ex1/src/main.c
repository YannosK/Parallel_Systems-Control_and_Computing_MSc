#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "game_of_life.h"

void argument_parse_error_message(char *program_name) {
    printf("Usage: %s <generations> <grid>\n", program_name);
    printf("\nArguments:\n");
    printf(" - generations: the number of generations to simulate.\n");
    printf(" - grid: the size of the grid (e.g. 10 for 10x10 grid).\n");
}

int main(int argc, char *argv[]) {
    // Function return value
    int ret;

    // Parse arguments
    if(argc != 3) {
        argument_parse_error_message(argv[0]);
        return 1;
    }

    int generations = strtol(argv[1], NULL, 10);
    int grid = strtol(argv[2], NULL, 10);

    // Initialize MPI
    int comm_sz, comm_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

    // Initialize state
    game_of_life_t gol;
    ret = gol_init(&gol, grid / comm_sz, grid);
    check_errors(
        ret, "gol_init", "unable to initialize the game of life", MPI_COMM_WORLD
    );

// Fill the input matrix
#ifdef DEBUG
    int max_string_len = 100;

    char filename[max_string_len];
    snprintf(filename, max_string_len, "data/test/%d.txt", grid);

    ret = gol_parse_input_from_file(
        &gol, filename, grid, comm_sz, comm_rank, MPI_COMM_WORLD
    );
    check_errors(
        ret, "gol_parse_input_from_file", "unable to parse input from file",
        MPI_COMM_WORLD
    );
#endif
#ifndef DEBUG
    ret = gol_random_input(&gol, grid, comm_sz, comm_rank, MPI_COMM_WORLD);
    check_errors(
        ret, "gol_random_input", "unable to generate random input",
        MPI_COMM_WORLD
    );
#endif

    double start, end;
    double local_elapsed, elapsed;

    // Execute the game of life
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    gol_execute(&gol, comm_sz, comm_rank, MPI_COMM_WORLD, generations);
    end = MPI_Wtime();
    elapsed = end - start;
    MPI_Reduce(
        &local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD
    );

    // TODO: uncomment to print the grid after the execution
    // ret = gol_print(&gol, grid, comm_sz, comm_rank, MPI_COMM_WORLD);
    // check_errors(
    //     ret, "gol_print", "unable to print the game of life grid",
    //     MPI_COMM_WORLD
    // );

    if(comm_rank == 0) {
        printf("Execution time: %.10fs\n", end - start);
    }

    // Destroy state
    ret = gol_destroy(&gol);
    check_errors(
        ret, "gol_destroy", "unable to destroy the game of life", MPI_COMM_WORLD
    );

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
