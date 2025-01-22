#include <stdio.h>
#include <stdlib.h>

#include "error.h"

void check_errors(int ret, char fname[], char message[], MPI_Comm comm) {
    int is_error;

    MPI_Allreduce(&ret, &is_error, 1, MPI_INT, MPI_MAX, comm);

    if(is_error) {
        int my_rank;
        MPI_Comm_rank(comm, &my_rank);
        if(my_rank == 0) {
            fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname, message);
            fflush(stderr);
        }
        MPI_Finalize();
        exit(-1);
    }
}
