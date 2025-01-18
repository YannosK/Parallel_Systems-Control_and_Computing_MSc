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
    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &comm_rank);

    // Initialize state
    game_of_life_t gol;
    ret = gol_init(&gol, grid, grid / comm_sz, grid, comm_sz, comm_rank, comm);
    check_errors(
        ret, "gol_init", "unable to initialize the game of life", comm
    );

// Fill the input matrix
#ifdef DEBUG
    int max_string_len = 100;

    char filename[max_string_len];
    snprintf(filename, max_string_len, "data/test/%d.txt", grid);

    ret = gol_parse_input_from_file(&gol, filename);
    check_errors(
        ret, "gol_parse_input_from_file", "unable to parse input from file",
        comm
    );
#endif
#ifndef DEBUG
    ret = gol_random_input(&gol);
    check_errors(
        ret, "gol_random_input", "unable to generate random input", comm
    );
#endif

    double start, end;
    double local_elapsed, elapsed;

    // Execute the game of life
    MPI_Barrier(comm);
    start = MPI_Wtime();
    gol_execute(&gol, generations);
    end = MPI_Wtime();
    elapsed = end - start;
    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

    // TODO: uncomment to print the grid after the execution
    // ret = gol_print(&gol);
    // check_errors(
    //     ret, "gol_print", "unable to print the game of life grid",
    //     comm
    // );

    if(comm_rank == 0) {
        printf("Execution time: %.10fs\n", end - start);
    }

    // Destroy state
    ret = gol_destroy(&gol);
    check_errors(
        ret, "gol_destroy", "unable to destroy the game of life", comm
    );

    // Finalize MPI
    MPI_Finalize();

    return 0;
}
