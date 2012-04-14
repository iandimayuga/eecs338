/* Ian Dimayuga
 * EECS 338 Assignment 6
 * list.h
 * Definitions for a Linked List implementation of a memory heap
 */

typedef struct list_node_struct {
    int start; //inclusive
    int end; //exclusive
    struct list_node_struct* next;
} list_node;

typedef struct {
    int start; //inclusive
    int end; //exclusive
    list_node* head;
} linked_list;

//prev is optional, leave as 0 if adding to head of list
void listAdd( linked_list* list, list_node* add, list_node* prev);

//prev is optional, leave as 0 if remove is the head of list
void listRemove( linked_list* list, list_node* remove, list_node* prev);

