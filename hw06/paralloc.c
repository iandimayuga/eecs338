/* 
 * Tim Henderson (tim.tadh@gmail.com)
 * EECS 338 - ParAlloc Skeleton Code
 * March 22, 2012
 * This code is placed in the public domain.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

#include <stddef.h>
#include <errno.h>

#include "paralloc.h"
#include "list.h"

struct linked_list* arenas;

/*
 * void heap_init()
 *
 * Performs any initialization work on the heap. (there may be no necessary work
 * but if there is put it here.)
 */
void heap_init() {
  //initialize pointer array
  arenas = malloc( sizeof(struct linked_list) * NUM_ARENAS);

  //initialize each arena's linked list
  int i;
  for( i = 0; i < NUM_ARENAS; i++) {
    list_node* block = malloc( sizeof(struct list_node));
    //partition the heap
    block->start = i * (HEAPSIZE / NUM_ARENAS);
    block->end = (i + 1) * (HEAPSIZE / NUM_ARENAS);
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
  pid_t pid = getpid();
  int arena = pid % NUM_ARENAS;

  //BEGIN CRITICAL SECTION
  semaphore(wait_mutex[arena]);

  list_node* current = arenas[arena].head;
  list_node* prev = NULL;
  //first-fit algorithm
  while( current->next) {
    int blocksize = current->end - current->start;
    if( blocksize >= amt) {
      //we found a fit
      int remainder = blocksize - amt;
      if( remainder) {
        //don't need to remove the node, just shift the start over
        current->start += amt;
      } else {
        //no remainder, must remove node
      }
    }
  }

  semaphore(signal_mutex[arena]);
  //END CRITICAL SECTION
  
  errno = ENOSYS;
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
  errno = ENOSYS;
  return -1;
}

