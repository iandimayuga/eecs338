/* 
 * Tim Henderson (tim.tadh@gmail.com)
 * EECS 338 - ParAlloc Skeleton Code
 * March 22, 2012
 * This code is placed in the public domain.
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>

#include "paralloc.h"

int attach() {
  heaploc = shmat(shmid, NULL, 0);
  if ((long)heaploc == -1) {
    perror("failed to attach to shared heap");
    return 1;
  }
  return 0;
}

int detach() {
  if (shmdt(heaploc) == -1) {
    perror("failed to detach from shared heap");
    return 1;
  }
  return 0;
}

int run_child(size_t allocations) {
  heap_ptr_t ptrs[allocations];
  int allocated = 0;
  int failures = 0;
  if (attach() != 0) {
    return 1;
  }
  {
    int i;
    for (i = 0; i < allocations; i++) {
      size_t size = rand() & 0x000000ff;
      heap_ptr_t ptr = paralloc(size);
      if (ptr < 0) {
        if (errno == EINVAL) {
          i -= 1;
          continue;
        } else {
          failures += 1;
          perror("paralloc failed");
          break;
        }
      } else {
        allocated += 1;
        ptrs[i] = ptr;
        char * mem = deref(char, ptr);
        int j;
        for (j = 0; j < size; j++) {
          mem[j] = (int)(rand() && 0x000000ff);
        }
      }
      usleep(40 * (rand() & 0x0000000f));
    }
  }
  usleep(100 * (rand() & 0x0000000f));
  {
    int i;
    for (i = 0; i < allocated; i++) {
      if (parfree(ptrs[i]) < 0) {
        failures += 1;
        perror("parfree failed");
      }
    }
    usleep(20 * (rand() & 0x0000000f));
  }
  failures += detach();
  return failures;
}

