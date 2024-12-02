#ifndef _PARALLEL_H_
#define _PARALLEL_H_

/*
 * Calculate the value of pi using a Monte Carlo method with multiple threads.
 *
 * Parameters:
 * - throws: The number of iterations.
 * - num_threads: The number of threads to use.
 * - pi: The value of pi.
 * - time: The time taken to calculate the value of pi.
 *
 * Returns:
 * - 0 if successful,
 * - 1 otherwise.
 */
int parallel(
    unsigned long long int throws, unsigned long int num_threads, double *pi,
    double *time
);

#endif