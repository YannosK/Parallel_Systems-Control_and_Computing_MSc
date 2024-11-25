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

// long long common = 0;         // common variable to be updated
long long *common_table = 0; // pointer to common variable table
// pthread_mutex_t common_mutex; // mutex for the common variable

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

    unsigned long thread;                    // thread iterator
    pthread_t *thread_handles;               // pointer to the array of thread handles
    double start, finish, elapsed;           // used for timing
    long long expected_el, expected_sum = 0; // expected values
    long long sum = 0;

    /***********************************
     *  Memory allocations
     ***********************************/

    thread_handles = (pthread_t *)malloc(thread_count * sizeof(pthread_t));
    if (thread_handles == NULL)
        return 2;

    common_table = (long long *)malloc(thread_count * sizeof(long long));
    if (common_table == NULL)
        return 2;

    /***********************************
     *  Initializations
     ***********************************/

    // pthread_mutex_init(&common_mutex, NULL);

    for (long long i = 0; i < (long long)thread_count; i++)
        common_table[i] = 0;

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

    expected_el = iterations;
    printf("Expected value of common table element: %lld\n", expected_el);
    for (long long i = 0; i < (long long)thread_count; i++)
        printf("Actual value element[%lld]: %lld\n", i, common_table[i]);

    for (long long i = 0; i < (long long)thread_count; i++)
        expected_sum += expected_el;
    printf("\nExpected value of sum of common table elements: %lld\n", expected_sum);
    for (long long i = 0; i < (long long)thread_count; i++)
        sum += common_table[i];
    printf("Actual value of sum of common table elements: %lld\n", sum);

    elapsed = finish - start;
    printf("\nElapsed time: %lf\n", elapsed);

    /***********************************
     *  Delete, destroy and deallocation
     ***********************************/

    // pthread_mutex_destroy(&common_mutex);
    free(thread_handles);
    free(common_table);

    return 0;
}

void *ThreadWork(void *rank)
{
    long my_rank = (long)rank;
    (void)my_rank;

    for (unsigned long i; i < iterations; i++)
    {
        // pthread_mutex_lock(&common_mutex);
        common_table[my_rank]++;
        // pthread_mutex_unlock(&common_mutex);
    }

    return NULL;
}