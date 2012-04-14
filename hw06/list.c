/* Ian Dimayuga
 * EECS 338 Assignment 6
 * list.c
 * Implementation of Linked List methods
 */

#include "list.h"

void listAdd( linked_list* list, list_node* add, list_node* prev) {
    if( prev) {
        add->next = prev->next;
        prev->next = add;
    } else {
        add->next = list->head;
        list->head = add;
    }
}

void listRemove( linked_list* list, list_node* remove, list_node* prev) {
    if( prev) prev->next = remove->next;
    else list->head = remove->next;
    remove->next = 0;
}
