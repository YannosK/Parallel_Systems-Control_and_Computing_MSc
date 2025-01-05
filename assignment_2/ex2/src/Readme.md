# Main Program C Code

This directory contains the main program of the current exercise.

The [`main.c`](./main.c) file is the main program that is executed and built. Build and run with the scripts or the make system. The main function takes command line arguments to specify wether to run the sequential or parallel version of the algorithms and wether to run the bask-substitution by row or column method. In either case it prints information on the command line, like the execution times, relative errors to other methods and speedup, for parallel execution.

while the [`equations_system.c`](./equations_system.c) contains the function definitions of the [`equationss_system.h`](../inc/equations_system.h) library,
that has the sequential and parallel code to be tested. For more information on these functions check the function definitions of the [`equationss_system.h`](../inc/equations_system.h) file.