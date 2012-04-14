/* Ian Dimayuga
 * EECS 338 Assignment 6
 * list.c
 * Implementation of Linked List methods
 */

#include "list.h"

void add( struct linked_list* list, struct list_node* add, struct list_node* prev) {
    if( prev) {
        add->next = prev->next;
        prev->next = add;
    } else {
        add->next = list->head;
        list->head = add;
    }
}

void remove( struct linked_list* list, struct list_node* remove, struct list_node* prev) {
    if( prev) prev->next = remove->next;
    else list->head = remove->next;
    remove->next = NULL;
}
