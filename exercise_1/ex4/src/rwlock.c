#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "rwlock.h"

typedef struct RWLock {
#ifdef DEFAULT
    pthread_rwlock_t lock;
#endif
#ifndef DEFAULT
    pthread_cond_t cond_read;
    pthread_cond_t cond_write;
    pthread_mutex_t mutex_rw;
    unsigned int executing_readers;
    unsigned int waiting_readers;
    unsigned int executing_writers;
    unsigned int waiting_writers;
    pthread_t writer;
#endif
} rwlock_s;

int rwlock_init(rwlock_t *rwlock) {
    if((*rwlock = malloc(sizeof(rwlock_s))) == NULL) {
        return 1;
    };

    int ret = 0;

#ifdef DEFAULT
    if((ret = pthread_rwlock_init(&(*rwlock)->lock, NULL)) != 0) {
        return ret;
    };
#endif
#ifndef DEFAULT
    if((ret = pthread_cond_init(&(*rwlock)->cond_read, NULL)) != 0) {
        return ret;
    };

    if((ret = pthread_cond_init(&(*rwlock)->cond_write, NULL)) != 0) {
        return ret;
    };

    if((ret = pthread_mutex_init(&(*rwlock)->mutex_rw, NULL)) != 0) {
        return ret;
    };

    (*rwlock)->executing_readers = 0;
    (*rwlock)->waiting_readers = 0;
    (*rwlock)->executing_writers = 0;
    (*rwlock)->waiting_writers = 0;
    (*rwlock)->writer = 0;
#endif

    return 0;
}

#ifndef DEFAULT
/*
 * The predicate that determines if the rwlock can be acquired for reading.
 *
 * Parameters:
 * - rwlock: the rwlock to be checked.
 *
 * Returns:
 * - 1 if the rwlock can be acquired for reading.
 * - 0 if the rwlock cannot be acquired for reading.
 */
int _rd_predicate(rwlock_t const *const rwlock) {
    int predicate;

#ifdef READER_PRIORITY_POLICY
    // There is no executing writer
    predicate = ((*rwlock)->executing_writers == 0);
#endif
#ifdef WRITER_PRIORITY_POLICY
    // There is no executing writer and no waiting writer
    predicate =
        (((*rwlock)->executing_writers == 0) &&
         ((*rwlock)->waiting_writers == 0));
#endif

    return predicate;
}
#endif

int rwlock_rdlock(rwlock_t *rwlock) {
    int ret = 0;

#ifdef DEFAULT
    if((ret = pthread_rwlock_rdlock(&(*rwlock)->lock)) != 0) {
        return ret;
    };
#endif
#ifndef DEFAULT
    if((ret = pthread_mutex_lock(&(*rwlock)->mutex_rw)) != 0) {
        return ret;
    };

#ifdef DEBUG
    static int calls = 0;

    printf("============================================\n");
    printf("RD_LOCK: (%d calls)\n", calls);
    printf("- Executing readers: %d\n", (*rwlock)->executing_readers);
    printf("- Waiting readers: %d\n", (*rwlock)->waiting_readers);
    printf("- Executing writers: %d\n", (*rwlock)->executing_writers);
    printf("- Waiting writers: %d\n", (*rwlock)->waiting_writers);
    printf("- Writer: %ld\n", (*rwlock)->writer);
    fflush(stdout);
#endif

    while(!_rd_predicate(rwlock) || (ret != 0)) {
        (*rwlock)->waiting_readers++;
        ret = pthread_cond_wait(&(*rwlock)->cond_read, &(*rwlock)->mutex_rw);
        (*rwlock)->waiting_readers--;
    }

    (*rwlock)->executing_readers++;

#ifdef DEBUG
    calls++;

    printf("--------------------------------------------\n");
    printf("- Executing readers: %d\n", (*rwlock)->executing_readers);
    printf("- Waiting readers: %d\n", (*rwlock)->waiting_readers);
    printf("- Executing writers: %d\n", (*rwlock)->executing_writers);
    printf("- Waiting writers: %d\n", (*rwlock)->waiting_writers);
    printf("- Writer: %ld\n", (*rwlock)->writer);
    fflush(stdout);
#endif

    if((ret = pthread_mutex_unlock(&(*rwlock)->mutex_rw)) != 0) {
        return ret;
    };
#endif

    return 0;
}

#ifndef DEFAULT
/*
 * The predicate that determines if the rwlock can be acquired for writing.
 *
 * Parameters:
 * - rwlock: the rwlock to be checked.
 *
 * Returns:
 * - 1 if the rwlock can be acquired for writing.
 * - 0 if the rwlock cannot be acquired for writing.
 */
int _rw_predicate(rwlock_t const *const rwlock) {
    int predicate;

#ifdef READER_PRIORITY_POLICY
    // There is no executing or waiting reader and no executing writer
    predicate = ((*rwlock)->executing_readers == 0) &&
                ((*rwlock)->waiting_readers == 0) &&
                ((*rwlock)->executing_writers == 0);
#endif
#ifdef WRITER_PRIORITY_POLICY
    // There is no executing reader and no executing writer
    predicate = ((*rwlock)->executing_readers == 0) &&
                ((*rwlock)->executing_writers == 0);
#endif

    return predicate;
}
#endif

int rwlock_wrlock(rwlock_t *rwlock) {
#ifdef DEFAULT
    return pthread_rwlock_wrlock(&(*rwlock)->lock);
#endif
#ifndef DEFAULT
    int ret = 0;

    if((ret = pthread_mutex_lock(&(*rwlock)->mutex_rw)) != 0) {
        return ret;
    };

#ifdef DEBUG
    static int calls = 0;

    printf("============================================\n");
    printf("WR_LOCK: (%d calls)\n", calls);
    printf("- Executing readers: %d\n", (*rwlock)->executing_readers);
    printf("- Waiting readers: %d\n", (*rwlock)->waiting_readers);
    printf("- Executing writers: %d\n", (*rwlock)->executing_writers);
    printf("- Waiting writers: %d\n", (*rwlock)->waiting_writers);
    printf("- Writer: %ld\n", (*rwlock)->writer);
    fflush(stdout);
#endif

    while(!_rw_predicate(rwlock) || (ret != 0)) {
        (*rwlock)->waiting_writers++;
        ret = pthread_cond_wait(&(*rwlock)->cond_write, &(*rwlock)->mutex_rw);
        (*rwlock)->waiting_writers--;
    }

    (*rwlock)->executing_writers = 1;
    (*rwlock)->writer = pthread_self();

#ifdef DEBUG
    calls++;

    printf("--------------------------------------------\n");
    printf("- Executing readers: %d\n", (*rwlock)->executing_readers);
    printf("- Waiting readers: %d\n", (*rwlock)->waiting_readers);
    printf("- Executing writers: %d\n", (*rwlock)->executing_writers);
    printf("- Waiting writers: %d\n", (*rwlock)->waiting_writers);
    printf("- Writer: %ld\n", (*rwlock)->writer);
    fflush(stdout);
#endif

    if((ret = pthread_mutex_unlock(&(*rwlock)->mutex_rw)) != 0) {
        return ret;
    };
#endif

    return 0;
}

#ifndef DEFAULT
/*
 * The predicate that determines if the unlocking writer should signal a waiting
 * reader.
 *
 * Parameters:
 * - rwlock: the rwlock to be checked.
 *
 * Returns:
 * - 1 if the unlocking writer should signal a waiting reader.
 * - 0 if the unlocking writer should not signal a waiting reader.
 */
int _uwr_signal_rd(rwlock_t const *const rwlock) {
    int signal_rd;

#ifdef READER_PRIORITY_POLICY
    signal_rd = ((*rwlock)->waiting_readers > 0);
#endif
#ifdef WRITER_PRIORITY_POLICY
    signal_rd =
        ((*rwlock)->waiting_readers > 0) && ((*rwlock)->waiting_writers == 0);
#endif

    return signal_rd;
}

/*
 * The predicate that determines if the unlocking writer should signal a waiting
 * writer.
 *
 * Parameters:
 * - rwlock: the rwlock to be checked.
 *
 * Returns:
 * - 1 if the unlocking writer should signal a waiting writer.
 * - 0 if the unlocking writer should not signal a waiting writer.
 */
int _uwr_signal_wr(rwlock_t const *const rwlock) {
    int signal_wr;

#ifdef READER_PRIORITY_POLICY
    signal_wr = ((*rwlock)->waiting_readers == 0) &&
                ((*rwlock)->executing_readers == 0) &&
                ((*rwlock)->waiting_writers > 0);
#endif
#ifdef WRITER_PRIORITY_POLICY
    signal_wr =
        ((*rwlock)->executing_readers == 0) && ((*rwlock)->waiting_writers > 0);
#endif

    return signal_wr;
}

/*
 * The predicate that determines if the unlocking reader should signal a waiting
 * reader.
 *
 * Parameters:
 * - rwlock: the rwlock to be checked.
 *
 * Returns:
 * - 1 if the unlocking reader should signal a waiting reader.
 * - 0 if the unlocking reader should not signal a waiting reader.
 */
int _urd_signal_rd(rwlock_t const *const rwlock) {
    int signal_rd;

#ifdef READER_PRIORITY_POLICY
    signal_rd =
        (((*rwlock)->waiting_readers > 0) && ((*rwlock)->executing_writers == 0)
        );
#endif
#ifdef WRITER_PRIORITY_POLICY
    signal_rd =
        (((*rwlock)->waiting_readers > 0) &&
         ((*rwlock)->waiting_writers == 0) &&
         ((*rwlock)->executing_writers == 0));
#endif

    return signal_rd;
}

/*
 * The predicate that determines if the unlocking reader should signal a waiting
 * writer.
 *
 * Parameters:
 * - rwlock: the rwlock to be checked.
 *
 * Returns:
 * - 1 if the unlocking reader should signal a waiting writer.
 * - 0 if the unlocking reader should not signal a waiting writer.
 */
int _urd_signal_wr(rwlock_t const *const rwlock) {
    int signal_wr;

#ifdef READER_PRIORITY_POLICY
    signal_wr = ((*rwlock)->waiting_readers == 0) &&
                ((*rwlock)->executing_readers == 0) &&
                ((*rwlock)->waiting_writers > 0) &&
                ((*rwlock)->executing_writers == 0);
#endif
#ifdef WRITER_PRIORITY_POLICY
    signal_wr = ((*rwlock)->executing_readers == 0) &&
                ((*rwlock)->waiting_writers > 0) &&
                ((*rwlock)->executing_writers == 0);
#endif

    return signal_wr;
}
#endif

int rwlock_unlock(rwlock_t *rwlock) {
#ifdef DEFAULT
    return pthread_rwlock_unlock(&(*rwlock)->lock);
#endif
#ifndef DEFAULT
    int ret = 0;

    if((ret = pthread_mutex_lock(&(*rwlock)->mutex_rw)) != 0) {
        return ret;
    };

#ifdef DEBUG
    static int calls_writer = 0;
    static int calls_reader = 0;

    printf("============================================\n");
    printf(
        "UNLOCK: (%d calls_writer, %d calls_reader)\n", calls_writer,
        calls_reader
    );
    printf("- Executing readers: %d\n", (*rwlock)->executing_readers);
    printf("- Waiting readers: %d\n", (*rwlock)->waiting_readers);
    printf("- Executing writers: %d\n", (*rwlock)->executing_writers);
    printf("- Waiting writers: %d\n", (*rwlock)->waiting_writers);
    printf("- Writer: %ld\n", (*rwlock)->writer);
    fflush(stdout);
#endif

    // The first condition is mandatory because the writer value on the rwlock
    // struct don’t get reset when we unlock the rwlock. This is because the
    // pthread_t type isn’t specified by the standard and can also be a struct,
    // instead of an integer, in some implementations.
    int is_writer = ((*rwlock)->executing_writers == 1) &&
                    (pthread_equal((*rwlock)->writer, pthread_self()) != 0);

    if(is_writer) { // Writer
        (*rwlock)->executing_writers = 0;
#ifdef DEBUG
        calls_writer++;
        printf("--------------------------------------------\n");
        printf("WR_UNLOCK: %ld\n", pthread_self());
        fflush(stdout);
#endif

        if(_uwr_signal_rd(rwlock)) {
            pthread_cond_broadcast(&(*rwlock)->cond_read);
        }

        if(_uwr_signal_wr(rwlock)) {
            pthread_cond_signal(&(*rwlock)->cond_write);
        }

    } else { // Reader
        ((*rwlock)->executing_readers)--;
#ifdef DEBUG
        calls_reader++;
        printf("--------------------------------------------\n");
        printf("RD_UNLOCK: %ld\n", pthread_self());
        fflush(stdout);
#endif
    }

    if(_urd_signal_rd(rwlock)) {
        pthread_cond_broadcast(&(*rwlock)->cond_read);
    }

    if(_urd_signal_wr(rwlock)) {
        pthread_cond_signal(&(*rwlock)->cond_write);
    }

#ifdef DEBUG
    printf("--------------------------------------------\n");
    printf("- Executing readers: %d\n", (*rwlock)->executing_readers);
    printf("- Waiting readers: %d\n", (*rwlock)->waiting_readers);
    printf("- Executing writers: %d\n", (*rwlock)->executing_writers);
    printf("- Waiting writers: %d\n", (*rwlock)->waiting_writers);
    printf("- Writer: %ld\n", (*rwlock)->writer);
    fflush(stdout);
#endif

    if((ret = pthread_mutex_unlock(&(*rwlock)->mutex_rw)) != 0) {
        return ret;
    };
#endif

    return 0;
}

int rwlock_destroy(rwlock_t *rwlock) {
    int ret = 0;

#ifdef DEFAULT
    if((ret = pthread_rwlock_destroy(&(*rwlock)->lock)) != 0) {
        return ret;
    };
#endif
#ifndef DEFAULT
    if((ret = pthread_cond_destroy(&(*rwlock)->cond_read)) != 0) {
        return ret;
    };

    if((ret = pthread_cond_destroy(&(*rwlock)->cond_write)) != 0) {
        return ret;
    };

    if((ret = pthread_mutex_destroy(&(*rwlock)->mutex_rw)) != 0) {
        return ret;
    };
#endif

    free(*rwlock);
    *rwlock = NULL;

    return ret;
}