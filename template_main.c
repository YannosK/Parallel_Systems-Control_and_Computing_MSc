#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

////////////////////////////////
// Local includes
///////////////////////////////

////////////////////////////////
// Private defines
///////////////////////////////

////////////////////////////////
// Global Variables
///////////////////////////////

unsigned long thread_count;

////////////////////////////////
// Function Definitions
///////////////////////////////

void *ThreadWork(void *rank);

int main(int argc, char *argv[])
{
    /***********************************
     *  Argument check
     ***********************************/

    if (argc != 2)
    {
        printf("Usage: ./main <threadsnum>\n");
        return 1;
    }

    thread_count = strtoul(argv[1], NULL, 10);

    /***********************************
     *  Local variables
     ***********************************/

    unsigned long thread;
    pthread_t *thread_handles;

    /***********************************
     *  Memory allocations
     ***********************************/

    thread_handles = (pthread_t *)malloc(thread_count * sizeof(pthread_t));
    if (thread_handles == NULL)
        return 2;

    /***********************************
     *  Initializations
     ***********************************/

    for (thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL, ThreadWork, (void *)thread);

    /***********************************
     *  Thread join
     ***********************************/

    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);

    /***********************************
     *  Final calculations
     ***********************************/

    /***********************************
     *  Delete, destroy and deallocation
     ***********************************/

    free(thread_handles);

    return 0;
}

void *ThreadWork(void *rank)
{
    long my_rank = (long)rank;

    return NULL;
}