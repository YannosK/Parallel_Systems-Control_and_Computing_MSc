#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_of_life.h"
#include "timer.h"

void argument_parse_error_message(char *program_name) {
    printf("Usage: %s <generations> <grid> <mode> <threads>\n", program_name);
    printf("\nArguments:\n");
    printf(" - generations: the number of generations to simulate.\n");
    printf(" - grid: the size of the grid (e.g. 10 for 10x10 grid).\n");
    printf(" - mode: (0) serial or (1) parallel.\n");
    printf(" - threads: the number of threads in parallel mode.\n");
}

int main(int argc, char *argv[]) {
    // Parse arguments
    if(argc != 5) {
        argument_parse_error_message(argv[0]);
        return 1;
    }

    int generations = strtol(argv[1], NULL, 10);
    int grid = strtol(argv[2], NULL, 10);
    int mode = strtol(argv[3], NULL, 10);
    int threads = strtol(argv[4], NULL, 10);

    // Initialize state
    game_of_life_t gol;
    if(gol_init(&gol, grid) != 0) {
        fprintf(stderr, "Error: unable to initialize the game of life.\n");
        return 1;
    }

// Fill the input matrix
#ifdef DEBUG
    int max_string_len = 100;

    char filename[max_string_len];
    snprintf(filename, max_string_len, "data/test/%d.txt", grid);

    if(gol_parse_input_from_file(&gol, filename) != 0) {
        fprintf(stderr, "Error: unable to parse input from %s.\n", filename);
        return 1;
    };
#endif
#ifndef DEBUG
    gol_random_input(&gol);
#endif

    double start, end;

    // Execute the game of life
    if(mode == 0) {
        GET_TIME(start);
        gol_execute(&gol, generations);
        GET_TIME(end);
    } else {
        start = omp_get_wtime();
        gol_execute_parallel(&gol, generations, threads);
        end = omp_get_wtime();
    }

    printf("Execution time: %.10fs\n", end - start);

    // Destroy state
    if(gol_destroy(&gol) != 0) {
        fprintf(stderr, "Error: unable to destroy the game of life.\n");
        return 1;
    }

    return 0;
}
