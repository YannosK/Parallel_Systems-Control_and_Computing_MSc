#include <stdio.h>
#include <stdlib.h>

#include "linkedlist.h"

/*
 * Check if list is empty.
 *
 * Returns:
 * - 0 if list is not empty.
 * - 1 if list is empty.
 */
int _is_empty(void);

/* Struct for list nodes */
struct list_node_s {
    int data;
    struct list_node_s *next;
};

/* The head of the list */
struct list_node_s *head = NULL;

int insert(int value) {
    struct list_node_s *curr = head;
    struct list_node_s *pred = NULL;
    struct list_node_s *temp;
    int rv = 1;

    while(curr != NULL && curr->data < value) {
        pred = curr;
        curr = curr->next;
    }

    if(curr == NULL || curr->data > value) {
        temp = malloc(sizeof(struct list_node_s));
        temp->data = value;
        temp->next = curr;
        if(pred == NULL)
            head = temp;
        else
            pred->next = temp;
    } else { /* value in list */
        rv = 0;
    }

    return rv;
}

void print(void) {
    struct list_node_s *temp;

    printf("list = ");

    temp = head;
    while(temp != (struct list_node_s *)NULL) {
        printf("%d ", temp->data);
        temp = temp->next;
    }
    printf("\n");
}

int member(int value) {
    struct list_node_s *temp;

    temp = head;
    while(temp != NULL && temp->data < value)
        temp = temp->next;

    if(temp == NULL || temp->data > value) {
#ifdef DEBUG
        printf("%d is not in the list\n", value);
#endif
        return 0;
    } else {
#ifdef DEBUG
        printf("%d is in the list\n", value);
#endif
        return 1;
    }
}

int delete(int value) {
    struct list_node_s *curr = head;
    struct list_node_s *pred = NULL;
    int rv = 1;

    /* Find value */
    while(curr != NULL && curr->data < value) {
        pred = curr;
        curr = curr->next;
    }

    if(curr != NULL && curr->data == value) {
        if(pred == NULL) { /* first element in list */
            head = curr->next;
#ifdef DEBUG
            printf("Freeing %d\n", value);
#endif
            free(curr);
        } else {
            pred->next = curr->next;
#ifdef DEBUG
            printf("Freeing %d\n", value);
#endif
            free(curr);
        }
    } else { /* Not in list */
        rv = 0;
    }

    return rv;
}

void free_list(void) {
    struct list_node_s *current;
    struct list_node_s *following;

    if(_is_empty())
        return;
    current = head;
    following = current->next;
    while(following != NULL) {
#ifdef DEBUG
        printf("Freeing %d\n", current->data);
#endif
        free(current);
        current = following;
        following = current->next;
    }
#ifdef DEBUG
    printf("Freeing %d\n", current->data);
#endif
    free(current);
}

int _is_empty(void) {
    if(head == NULL)
        return 1;
    else
        return 0;
}