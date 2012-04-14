/* 
 * Tim Henderson (tim.tadh@gmail.com)
 * EECS 338 - ParAlloc Skeleton Code
 * March 22, 2012
 * This code is placed in the public domain.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

#include <stddef.h>
#include <errno.h>

#include "paralloc.h"
#include "list.h"

//list of allocated blocks
linked_list** arenas;

/*
 * void heap_init()
 *
 * Performs any initialization work on the heap. (there may be no necessary work
 * but if there is put it here.)
 */
void heap_init() {
  //initialize pointer array
  arenas = malloc( sizeof( linked_list*) * NUM_ARENAS);

  //initialize each arena's linked list
  int i;
  for( i = 0; i < NUM_ARENAS; i++) {
    arenas[i] = malloc( sizeof( linked_list));
    //partition the heap
    arenas[i]->start = i * (HEAPSIZE / NUM_ARENAS);
    arenas[i]->end = (i + 1) * (HEAPSIZE / NUM_ARENAS);
    arenas[i]->head = 0;
  }
}

/*
 * heap_ptr_t paralloc(size_t amt) 
 *    @param amt : the amount of memory to allocate
 *    @returns on success : an offset (relative to the global `heaploc` defined 
 *                          in `paralloc.h`)
 *    @return on error : -1 (and set errno to the appropriate value).
 *    
 *    Allocates the requested amount of memory from the shared memory segment.
 */
heap_ptr_t paralloc(size_t amt) { 
  if( amt <= 0) {
    //bad argument
    errno = EINVAL;
    return -1;
  }
  pid_t pid = getpid();
  int arena = pid % NUM_ARENAS;

  //BEGIN CRITICAL SECTION
  semaphore(wait_mutex[arena]);

  int offset = 0;
  linked_list* list = arenas[arena];
  list_node* current = list->head;
  list_node* prev = 0;

  //first-fit algorithm
  while( current) {
    int freesize = current->start - offset;
    if( freesize >= amt) {
      //we found a fit, need to make a node
      list_node* add = malloc( sizeof( list_node));
      add->start = offset;
      add->end = offset + amt;
      add->next = 0;

      //add node to list
      listAdd( list, add, prev);

      semaphore( signal_mutex[arena]);
      return offset;
    }
    offset = current->end;
    prev = current;
    current = current->next;
  }

  if( list->end - offset >= amt) {
    list_node* add = malloc( sizeof( list_node));
    add->start = offset;
    add->end = offset + amt;
    add->next = 0;
    listAdd( list, add, prev);

    semaphore( signal_mutex[arena]);
    return offset;
  }

  semaphore(signal_mutex[arena]);
  //END CRITICAL SECTION

  //out of memory error
  errno = ENOMEM;
  return -1;
}

/*
 * int parfree(heap_ptr_t offset) 
 *
 *    @param offset : the offset pointing to the memory you want to free (as
 *                    returned by paralloc)
 *    @returns on success : 0
 *    @returns on error : -1
 *
 *    Frees the memory.
 */
int parfree(heap_ptr_t offset) {
  if( offset < 0 || offset >= HEAPSIZE) {
    //bad argument
    errno = EINVAL;
    return -1;
  }
  pid_t pid = getpid();
  int arena = pid % NUM_ARENAS;

  //BEGIN CRITICAL SECTION
  semaphore(wait_mutex[arena]);

  linked_list* list = arenas[arena];
  list_node* current = list->head;
  list_node* prev = 0;
  //first-fit algorithm
  while( current) {
    if( current->start == offset) {
      //this is the block to be freed, remove from linked list
      listRemove( list, current, prev);
      free(current);

      //because the list only keeps track of allocated blocks,
      //this algorithm automatically coalesces.

      semaphore( signal_mutex[arena]);
      return 0;
    }
    if( current->start > offset) {
      //this offset has not been allocated, this is bad!
      break;
    }
    prev = current;
    current = current->next;
  }
  
  semaphore(signal_mutex[arena]);
  //END CRITICAL SECTION

  errno = EINVAL;
  return -1;
}

