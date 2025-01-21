# Main Program C Code

This directory contains the parallel program of the current exercise.

The [`main.c`](./src/main.c) file is the main program that is executed and built. Build and run with the scripts or the make system.

The main function takes command line arguments to specify the problem size, which is the dimension of the matrix and the vector to be multiplied. It must also be run with the gcc
wrapper mpicc dictating the number of processes with the flag -n and even specifying a file named 'machines' (if it exists) with working mpi machines, with the flag -f.

The operation is performed by functions local to the [`main.c`](./src/main.c) file and others provided from the custom [`linalgebra.h`](./inc/linalgebra.h) file.
See the function contracts for information. Local [`main.c`](./src/main.c) functions are extensively commented in their definition.

The program prints the execution time and the time it took to share local data among all the MPI proceses.
Timings operations are done by timing APIs of MPI.

If the line `#define VERBOSE` is included in the [`main.c`](./src/main.c) file then the result vector will be printed.
If the line `#define DEBUG` is included in the [`main.c`](./src/main.c) file then the initial matrix and vector are printed too.
