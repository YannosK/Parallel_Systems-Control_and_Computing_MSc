#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

////////////////////////////////
// Local includes
///////////////////////////////

#include "timer.h"

////////////////////////////////
// Private defines
///////////////////////////////

////////////////////////////////
// Global Variables
///////////////////////////////

unsigned long thread_count; // number of threads
unsigned long iterations;   // number of iterations of for loops

long long common = 0;         // common variable to be updated
pthread_mutex_t common_mutex; // mutex for the common variable

////////////////////////////////
// Function Definitions
///////////////////////////////

void *ThreadWork(void *rank);

int main(int argc, char *argv[])
{
    /***********************************
     *  Argument check
     ***********************************/

    if (argc != 3)
    {
        printf("Usage: ./main <threadsnum> <iterations>\n");
        return 1;
    }

    thread_count = strtoul(argv[1], NULL, 10);
    iterations = strtoul(argv[2], NULL, 10);

    /***********************************
     *  Local variables
     ***********************************/

    unsigned long thread;          // thread iterator
    pthread_t *thread_handles;     // pointer to the array of thread handles
    double start, finish, elapsed; // used for timing
    long long expected;            // expected value of common variable after execution

    /***********************************
     *  Memory allocations
     ***********************************/

    thread_handles = (pthread_t *)malloc(thread_count * sizeof(pthread_t));
    if (thread_handles == NULL)
        return 2;

    /***********************************
     *  Initializations
     ***********************************/

    pthread_mutex_init(&common_mutex, NULL);

    GET_TIME(start);
    for (thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL, ThreadWork, (void *)thread);

    /***********************************
     *  Thread join
     ***********************************/

    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);
    GET_TIME(finish);

    /***********************************
     *  Final calculations
     ***********************************/

    expected = iterations * thread_count;
    printf("Expected value of common variable: %lld\n", expected);
    printf("Actual value of common variable: %lld\n", common);

    elapsed = finish - start;
    printf("\nElapsed time: %lf\n", elapsed);

    /***********************************
     *  Delete, destroy and deallocation
     ***********************************/

    pthread_mutex_destroy(&common_mutex);
    free(thread_handles);

    return 0;
}

void *ThreadWork(void *rank)
{
    long my_rank = (long)rank;
    (void)my_rank;

    for (unsigned long i = 0; i < iterations; i++)
    {
        pthread_mutex_lock(&common_mutex);
        common++;
        pthread_mutex_unlock(&common_mutex);
    }

    return NULL;
}