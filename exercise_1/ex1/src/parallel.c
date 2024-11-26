#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "parallel.h"
#include "timer.h"

pthread_mutex_t mutex;
unsigned long long int throws_global;
unsigned long int num_threads_global;
double pi_global;

void *thread_work(void *rank) {
    const unsigned int _rank = *(unsigned int *)rank;
    unsigned int seed = _rank;
    const unsigned long long int throws = throws_global / num_threads_global;

    unsigned long long int throw;
    unsigned long long int throws_in_circle = 0;

    double x, y, square_distance;

    for(throw = 0; throw < throws; throw ++) {

        x = 2.0 * rand_r(&seed) / RAND_MAX - 1;
        y = 2.0 * rand_r(&seed) / RAND_MAX - 1;

        square_distance = x * x + y * y;

        if(square_distance <= 1) {
            throws_in_circle++;
        }
    }

    throws_in_circle *= 4;

    pthread_mutex_lock(&mutex);
    pi_global += throws_in_circle;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int parallel(
    unsigned long long int throws, unsigned long int num_threads, double *pi,
    double *time
) {
    double start, end;

    GET_TIME(start);

    throws_global = throws;
    num_threads_global = num_threads;

    pthread_t *thread_handles;
    if((thread_handles = calloc(num_threads, sizeof(pthread_t))) == NULL) {
        return 1;
    };

    unsigned long int *thread_args =
        calloc(num_threads, sizeof(unsigned long int));

    if(pthread_mutex_init(&mutex, NULL) != 0) {
        return 1;
    };

    unsigned long int thread;

    for(thread = 0; thread < num_threads; thread++) {
        thread_args[thread] = thread;
        if(pthread_create(
               &thread_handles[thread], NULL, thread_work, &thread_args[thread]
           ) != 0) {
            return 1;
        };
    }

    for(thread = 0; thread < num_threads; thread++) {
        if(pthread_join(thread_handles[thread], NULL) != 0) {
            return 1;
        };
    }

    pi_global /= throws;

    free(thread_handles);
    free(thread_args);

    if(pthread_mutex_destroy(&mutex) != 0) {
        return 1;
    }

    GET_TIME(end);

    *time = end - start;
    *pi = pi_global;

    return 0;
}