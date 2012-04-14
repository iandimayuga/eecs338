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
    list_node* block = malloc( sizeof( list_node));
    //partition the heap
    block->start = i * (HEAPSIZE / NUM_ARENAS);
    block->end = (i + 1) * (HEAPSIZE / NUM_ARENAS);

    //add to list
    listAdd( arenas[i], block, 0);
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
  int offset = -1;

  //BEGIN CRITICAL SECTION
  semaphore(wait_mutex[arena]);

  linked_list* list = arenas[arena];
  list_node* current = list->head;
  list_node* prev = 0;
  //first-fit algorithm
  while( current) {
    int blocksize = current->end - current->start;
    if( blocksize >= amt) {
      //we found a fit, set the offset
      offset = current->start;

      //find how much remaining in the block
      int remainder = blocksize - amt;
      if( remainder) {
        //don't need to remove the node, just shift the start over
        current->start += amt;
      } else {
        //no remainder, must remove node
        listRemove( list, current, prev);
        free(current);
      }
      break;
    }
    prev = current;
    current = current->next;
  }

  semaphore(signal_mutex[arena]);
  //END CRITICAL SECTION

  if( offset < 0) {
    //out of memory error
    errno = ENOMEM;
    return -1;
  }

  return offset;
  
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
    if( current->start <= offset && current->end > offset) {
      //block is already free, this is bad!
      errno = EINVAL;
      return -1;
    }
    if( current->start > offset) {
      if( prev && prev->end == offset)

    }
    prev = current;
    current = current->next;
  }
  
  semaphore(signal_mutex[arena]);
  //END CRITICAL SECTION

  return 0;
}

