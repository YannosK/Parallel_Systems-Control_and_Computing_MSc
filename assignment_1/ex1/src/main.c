#include <stdio.h>
#include <stdlib.h>

#include "parallel.h"
#include "serial.h"

int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <throws> <num_threads>\n", argv[0]);
        return 1;
    }

    unsigned long long int throws;
    unsigned long int num_threads;
    double pi, time;

    throws = strtoll(argv[1], NULL, 10);
    num_threads = strtoll(argv[2], NULL, 10);

    serial(throws, &pi, &time);

    printf("Serial Monte Carlo: ");
    printf("Pi: %f, ", pi);
    printf("Time: %f\n", time);

    if(parallel(throws, num_threads, &pi, &time) != 0) {
        fprintf(stderr, "Error: parallel Monte Carlo failed.\n");
        return 1;
    }

    printf("Parallel Monte Carlo: ");
    printf("Pi: %f, ", pi);
    printf("Time: %f\n", time);

    return 0;
}