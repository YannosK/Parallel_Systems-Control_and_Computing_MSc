# Sequential Program C Code

This directory contains the sequential program of the current exercise.

The [`main.c`](./src/main.c) file is the main program that is executed and built. Build and run with the scripts or the make system.

The main function takes command line arguments to specify the problem size, which is the dimension of the matrix and the vector to be multiplied.

The operation is performed by functions provided from th custom [`linalgebra.h`](./inc/linalgebra.h) file. See the header file for information on the functions and their arguments.

The program prints the execution time. Timings operations are provided by the [`timer.h`](./inc/timer.h) header file.

If the line `#define VERBOSE` is included in the [`main.c`](./src/main.c) file then the result vector will be printed.
If the line `#define DEBUG` is included in the [`main.c`](./src/main.c) file then the initial matrix and vector are printed too.
