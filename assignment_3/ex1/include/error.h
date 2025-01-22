#ifndef __ERROR_H__
#define __ERROR_H__

#include <mpi.h>

/*
 * Check whether any process has found an error.  If so,
 * print message and terminate all processes.  Otherwise,
 * continue execution.
 *
 * Taken from: https://ippbook.com
 *
 * Parameters:
 * - ret: 0 if no error occurred, 1 otherwise.
 * - fname: name of function.
 * - message: message to print if there's an error.
 * - comm: communicator containing processes.
 */
void check_errors(int ret, char fname[], char message[], MPI_Comm comm);

#endif
