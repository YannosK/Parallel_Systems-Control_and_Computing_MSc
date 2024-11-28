#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

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
size_t array_size;          // size of the common array
size_t elements;            // size of a group of elements of the array, that is the updated value and some padding elements with junk values

long long *common_table = 0; // pointer to common variable table

////////////////////////////////
// Function Definitions
///////////////////////////////

void *ThreadWork(void *rank);

int main(int argc, char *argv[])
{
    /***********************************
     *  Local variables
     ***********************************/

    unsigned long thread;                    // thread iterator
    pthread_t *thread_handles;               // pointer to the array of thread handles
    double start, finish, elapsed;           // used for timing
    long long expected_el, expected_sum = 0; // expected values
    long long sum = 0;
    size_t cache_line;

    /***********************************
     *  Argument check
     ***********************************/

    if (argc != 4)
    {
        printf("Usage: ./main <threadsnum> <iterations> <cachelinesize>\n");
        return 1;
    }

    thread_count = strtoul(argv[1], NULL, 10);
    iterations = strtoul(argv[2], NULL, 10);
    cache_line = strtoul(argv[3], NULL, 10);

    assert(cache_line >= sizeof(long long));
    assert((cache_line % sizeof(long long)) == 0);

    /***********************************
     *  Memory allocations
     ***********************************/

    thread_handles = (pthread_t *)malloc(thread_count * sizeof(pthread_t));
    if (thread_handles == NULL)
        return 2;

    array_size = thread_count * cache_line;
    common_table = (long long *)malloc(array_size);
    if (common_table == NULL)
        return 2;

    /***********************************
     *  Initializations
     ***********************************/

    elements = cache_line / (sizeof(long long));

    for (thread = 0; thread < thread_count; thread++)
        common_table[thread * elements] = 0;

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
        printf("Actual value element[%lld]: %lld\n", i, common_table[i * elements]);

    for (long long i = 0; i < (long long)thread_count; i++)
        expected_sum += expected_el;
    printf("\nExpected value of sum of common table elements: %lld\n", expected_sum);
    for (long long i = 0; i < (long long)thread_count; i++)
        sum += common_table[i * elements];
    printf("Actual value of sum of common table elements: %lld\n", sum);

    elapsed = finish - start;
    printf("\nElapsed time: %lf\n", elapsed);

    /***********************************
     *  Delete, destroy and deallocation
     ***********************************/

    free(thread_handles);
    free(common_table);

    return 0;
}

void *ThreadWork(void *rank)
{
    long my_rank = (long)rank;
    unsigned long my_index = my_rank * elements;

    for (unsigned long i = 0; i < iterations; i++)
    {
        common_table[my_index]++;
    }

    return NULL;
}