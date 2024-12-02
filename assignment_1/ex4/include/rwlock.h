typedef struct RWLock *rwlock_t;

/*
 * Initializes the rwlock.
 *
 * Parameters:
 * - rwlock: the rwlock to be initialized.
 *
 * Returns:
 * - 0 if the rwlock was initialized successfully.
 * - non-zero value if an error occurred.
 */
int rwlock_init(rwlock_t *rwlock);

/*
 * Locks the rwlock for reading.
 *
 * Parameters:
 * - rwlock: the rwlock to be locked.
 *
 * Returns:
 * - 0 if the rwlock was locked successfully.
 * - non-zero value if an error occurred.
 */
int rwlock_rdlock(rwlock_t *rwlock);

/*
 * Locks the rwlock for writing.
 *
 * Parameters:
 * - rwlock: the rwlock to be locked.
 *
 * Returns:
 * - 0 if the rwlock was locked successfully.
 * - non-zero value if an error occurred.
 */
int rwlock_wrlock(rwlock_t *rwlock);

/*
 * Unlocks the rwlock.
 *
 * Parameters:
 * - rwlock: the rwlock to be unlocked.
 *
 * Returns:
 * - 0 if the rwlock was unlocked successfully.
 * - non-zero value if an error occurred.
 */
int rwlock_unlock(rwlock_t *rwlock);

/*
 * Destroys the rwlock.
 *
 * Parameters:
 * - rwlock: the rwlock to be destroyed.
 *
 * Returns:
 * - 0 if the rwlock was destroyed successfully.
 * - non-zero value if an error occurred.
 */
int rwlock_destroy(rwlock_t *rwlock);