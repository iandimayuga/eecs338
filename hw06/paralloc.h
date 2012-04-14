/* 
 * Tim Henderson (tim.tadh@gmail.com)
 * EECS 338 - ParAlloc Skeleton Code
 * March 22, 2012
 * This code is placed in the public domain.
 */

#ifndef PARALLOC_SEEN
#define PARALLOC_SEEN


// Types
typedef int heap_ptr_t;

// Functions
void heap_init();
heap_ptr_t paralloc(size_t amt);
int parfree(heap_ptr_t);

// Macros
#define HEAPSIZE 4096*64 // in bytes | 4096 == one page
#define deref(T,o) ((T *)(heaploc + ((size_t)o)))

// Globals
int shmid;
void *heaploc;

//arena stuff
#define NUM_ARENAS 8

//semaphore stuff
#define WAIT -1
#define SIGNAL 1
int semkey;
struct sembuf* wait_mutex; //array of semaphores
struct sembuf* signal_mutex; //array of semaphores

#endif
