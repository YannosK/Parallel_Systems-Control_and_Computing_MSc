#ifndef _LINIKEDLIST_H_
#define _LINIKEDLIST_H_

/*
 * Insert value in a correct numerical location into list.
 *
 * Parameters:
 * - value: value to be inserted.
 *
 * Returns:
 * - 0 if value is in list.
 * - 1 if value is not in list.
 */
int insert(int value);

/*
 * Print the whole list.
 */
void print(void);

/*
 * Check if value is in list.
 *
 * Parameters:
 * - value: value to be checked.
 *
 * Returns:
 * - 0 if value is not in list.
 * - 1 if value is in list.
 */
int member(int value);

/*
 * Delete value from list.
 *
 * Parameters:
 * - value: value to be deleted.
 *
 * Returns:
 * - 0 if value is not in list.
 * - 1 if value is in list.
 */
int delete(int value);

/*
 * Free the whole list.
 */
void free_list(void);

#endif