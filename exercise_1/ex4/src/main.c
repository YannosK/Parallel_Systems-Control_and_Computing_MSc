#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "linkedlist.h"
#include "my_rand.h"
#include "timer.h"

/* Random ints are less than MAX_KEY */
const int MAX_KEY = 1e8;

/* Shared variables */
int thread_count;
int total_ops;
double insert_percent;
double search_percent;
double delete_percent;

pthread_rwlock_t rwlock;
pthread_mutex_t count_mutex;

int member_count = 0, insert_count = 0, delete_count = 0;

/*
 * Get the arguments from the command line. The arguments are:
 * -t: number of threads.
 * -k: keys that should be inserted in the main thread.
 * -o: operations that will be performed in total.
 * -s: percentage of search operations.
 * -i: percentage of insert operations.
 * -d: percentage of delete operations.
 * It is required that all the arguments are passed.
 *
 * Example:
 * main -t 4 -k 100000 -o 1000 -s 0.7 -i 0.2 -d 0.1
 *
 * Parameters:
 * - argc: number of arguments of main.
 * - argv: arguments of main.
 * - inserts_in_main_p: number of keys that should be inserted in the main
 * thread.
 *
 * Returns:
 * - 0 if the arguments were parsed successfully.
 * - 1 if an error occurred.
 */
int arg_parser(int argc, char *argv[], int *inserts_in_main_p);

void *thread_work(void *rank);

int main(int argc, char *argv[]) {
    int inserts_in_main;

    if(arg_parser(argc, argv, &inserts_in_main)) {
        return 1;
    }

    long i = 0;
    unsigned seed = 1;
    int key, success, attempts = 0;
    /* Try to insert inserts_in_main keys, but give up after */
    /* 2*inserts_in_main attempts.                           */
    while(i < inserts_in_main && attempts < 2 * inserts_in_main) {
        key = my_rand(&seed) % MAX_KEY;
        success = insert(key);
        attempts++;
        if(success) {
            i++;
        }
    }

    printf("Initial keys = %ld\n", i);

#ifdef OUTPUT
    printf("Before starting threads, list = \n");
    print();
    printf("\n");
#endif

    pthread_t *thread_handles;
    thread_handles = malloc(thread_count * sizeof(pthread_t));

    pthread_mutex_init(&count_mutex, NULL);
    pthread_rwlock_init(&rwlock, NULL);

    double start, finish;
    GET_TIME(start);
    for(i = 0; i < thread_count; i++) {
        pthread_create(&thread_handles[i], NULL, thread_work, (void *)i);
    }
    for(i = 0; i < thread_count; i++) {
        pthread_join(thread_handles[i], NULL);
    }
    GET_TIME(finish);

    printf("Elapsed time = %lf seconds\n", finish - start);
    printf("Total ops = %d\n", total_ops);
    printf("member ops = %d\n", member_count);
    printf("insert ops = %d\n", insert_count);
    printf("delete ops = %d\n", delete_count);

#ifdef OUTPUT
    printf("After threads terminate, list = \n");
    print();
    printf("\n");
#endif

    free_list();
    pthread_rwlock_destroy(&rwlock);
    pthread_mutex_destroy(&count_mutex);
    free(thread_handles);

    return 0;
}

void *thread_work(void *rank) {
    long my_rank = (long)rank;
    int ops_per_thread = total_ops / thread_count;

    int i, val;
    double which_op;
    unsigned seed = my_rank + 1;
    int my_member_count = 0, my_insert_count = 0, my_delete_count = 0;

    for(i = 0; i < ops_per_thread; i++) {
        which_op = my_drand(&seed);
        val = my_rand(&seed) % MAX_KEY;
        if(which_op < search_percent) {
            pthread_rwlock_rdlock(&rwlock);
            member(val);
            pthread_rwlock_unlock(&rwlock);
            my_member_count++;
        } else if(which_op < search_percent + insert_percent) {
            pthread_rwlock_wrlock(&rwlock);
            insert(val);
            pthread_rwlock_unlock(&rwlock);
            my_insert_count++;
        } else { /* delete */
            pthread_rwlock_wrlock(&rwlock);
            delete(val);
            pthread_rwlock_unlock(&rwlock);
            my_delete_count++;
        }
    }

    pthread_mutex_lock(&count_mutex);
    member_count += my_member_count;
    insert_count += my_insert_count;
    delete_count += my_delete_count;
    pthread_mutex_unlock(&count_mutex);

    return NULL;
}

int arg_parser(int argc, char *argv[], int *inserts_in_main_p) {
    if(argc < 13) {
        fprintf(
            stderr,
            "usage: %s "
            "-t <thread_count> "
            "-k <inserts_in_main> "
            "-o <total_ops> "
            "-s <search_percent> "
            "-i <insert_percent> "
            "-d <delete_percent>\n",
            argv[0]
        );
        return 1;
    }

    *inserts_in_main_p = 0;

    int c;

    // Search for argument dash
    while(--argc > 0 && (*++argv)[0] == '-') {
        // Get the argument character
        while((c = *++argv[0])) {
            switch(c) {
            // Thread count
            case 't':
                thread_count = atoi(*(++argv));
                argc--;
                break;
            // Keys inserted in the main thread
            case 'k':
                *inserts_in_main_p = atoi(*(++argv));
                argc--;
                break;
            // Operations in total
            case 'o':
                total_ops = atoi(*(++argv));
                argc--;
                break;
            // The percentage of search operations
            case 's':
                search_percent = atof(*(++argv));
                argc--;
                break;
            // The percentage of insert operations
            case 'i':
                insert_percent = atof(*(++argv));
                argc--;
                break;
            // The percentage of delete operations
            case 'd':
                delete_percent = atof(*(++argv));
                argc--;
                break;
            // Illegal option
            default:
                return 1;
            }
            // Search for next argument dash
            break;
        }
    }

    return 0;
}