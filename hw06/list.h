/* Ian Dimayuga
 * EECS 338 Assignment 6
 * list.h
 * Definitions for a Linked List implementation of a memory heap
 */

struct list_node {
    int start; //inclusive
    int end; //exclusive
    struct list_node* next;
};

struct linked_list {
    struct list_node* head;
};

//prev is optional, leave NULL if adding to head of list
void add( struct linked_list* list, struct list_node* add, struct list_node* prev);

//prev is optional, leave NULL if remove is the head of list
void remove( struct linked_list* list, struct list_node* remove, struct list_node* prev);

