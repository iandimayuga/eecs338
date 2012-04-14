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
#include <sys/shm.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "paralloc.h"
#include "child.h"

#define CHILDREN 25

// Exit Codes
const int FORK_FAIL = -1;
const int SHMGET_FAIL = -2;
const int SHMAT_FAIL = -3;
const int SHMDT_FAIL = -4;
const int SHMCTL_FAIL = -5;
const int WAIT_FAIL = -6;
const int CHILD_FAIL = -7;
const int OPEN_FAIL = -8;
const int READ_FAIL = -9;
const int CLOSE_FAIL = -10;
const int SEMGET_FAIL = -11;
const int SEMCTL_FAIL = -12;
const int SIGACTION_FAIL = -13;

// Am I the parent?
int parent = 1;

// Forward function declarations
void cleanup();
void fail(int);

// Semaphore Union Declaration
// Copied from the semctl man page (man semctl)
union semun {
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO
                              (Linux-specific) */
};

/* void seed()
 *
 * Seeds the random number generator from /dev/urandom
 */
void seed() {
  int urandfd;
  char randdata[sizeof(int)];
  if ((urandfd = open("/dev/urandom", O_RDONLY)) <  0) {
    perror("could not open /dev/urandom");
    fail(OPEN_FAIL);
  }
  if (read(urandfd, randdata, sizeof(randdata)) < sizeof(randdata)) {
    perror("could not read all the data from /dev/urandom");
    fail(READ_FAIL);
  }
  if (close(urandfd) < 0) {
    perror("closing /dev/urandom failed");
    fail(CLOSE_FAIL);
  }
  srand(*((int *)randdata));
}

/* void cleanup()
 *
 * Cleans up (as necessary) any allocated shared memory or semaphores.
 */
void cleanup() {
  if (parent == 1 && shmid >= 0) {
    if (shmctl(shmid, IPC_RMID, (struct shmid_ds *)NULL) < 0) {
      perror("shmctl failed to remove the shared memory");
      shmid = -1; // have to do this to prevent infinite mutual recursion
      fail(SHMCTL_FAIL);
    }
  }
  // remove semaphore group
  if (semkey >= 0) {
      if (semctl(semkey, 0, IPC_RMID) < 0) {
          perror("Error removing semaphores");
          semkey = -1; //prevent infinite mutual recursion
          fail(SEMCTL_FAIL);
      }
  }
}

/* void fail(int exit_status)
 *    @param exit_status : the exit status of the process
 *
 * Causes the program to fail (cleanly) with the given exit status
 */
void fail(int exit_status) {
  cleanup();
  _exit(exit_status);
}

/* void heap_create()
 *
 * Creates the shared memory "heap"
 */
void heap_create() {
  // Create shared memory segment
  // IPC_PRIVATE => don't use a key, make a new shared memory segment everytime
  // HEAPSIZE => The size of the "Heap" we are going to have. This is defined at
  //             the top of `main.c`
  // IPC_CREAT => Create the segment
  // IPC_EXCL => Fail if already created
  // 0666 => RW segment (for everyone)
  shmid = shmget(IPC_PRIVATE, HEAPSIZE, IPC_CREAT|IPC_EXCL|0666);
  if (shmid < 0) {
    perror("failed to allocate shared memory with shmget");
    fail(SHMGET_FAIL);
  }
  printf("shared memory id : %d\n", shmid);
}



/* void sem_create()
 *
 * create your semaphores here
 */
void sem_create() {
  semkey = semget(IPC_PRIVATE, NUM_ARENAS, IPC_CREAT | 0666);
  if (semkey < 0) {
      fail(SEMGET_FAIL);
  }

  //initialize semaphore arrays
  wait_mutex = malloc( NUM_ARENAS * sizeof( struct sembuf));
  signal_mutex = malloc( NUM_ARENAS * sizeof( struct sembuf));
  int i;
  for( i = 0; i < NUM_ARENAS; i++) {
    //initialize each semaphore
    wait_mutex[i].sem_num = i;
    wait_mutex[i].sem_op = WAIT;
    wait_mutex[i].sem_flg = 0;
    signal_mutex[i].sem_num = i;
    signal_mutex[i].sem_op = SIGNAL;
    signal_mutex[i].sem_flg = 0;
  }
}

/* void semaphore()
 * Performs the semaphore operation specified.
 */
void semaphore( struct sembuf op) {
    if( semop( semkey, &op, 1) < 0) {
        fail(SEMCTL_FAIL);
    }
}

/* int fork_kids(pid_t *kids)
 *    @oparam kids : an output parameter, an array of pids
 *    @returns on success : 0
 *    @returns on failure : a non-zero number
 *
 *    Forks the number of kids specified in CHILDREN and places their pids in
 *    the kids array.
 */
int fork_kids(pid_t *kids) {
  int i;
  int failed = 0;
  for (i = 0; i < CHILDREN; i++) {
    kids[i] = 0;
  }
  for (i = 0; i < CHILDREN; i++) {
    pid_t pid = fork();
    if (pid < 0) {
      failed = -1;
      perror("fork failed");
      break;
    } else if (pid == 0) {
      parent = 0; // I am not the parent.
      seed();
      _exit(run_child(rand() & 0x0000003f));
    } else {
      kids[i] = pid;
    }
  }
  return failed;
}

/* int waitkids(pid_t * kids)
 *    @param kids : a pointer to an array of pids
 *    @returns on success : 0
 *    @returns on failure : a non-zero number
 *
 *    Waits for the specified children to exit
 */
int waitkids(pid_t *kids) {
  int i;
  int failures = 0;
  for (i = 0; i < CHILDREN; i++) {
    pid_t pid = kids[i];
    if (pid == 0) { continue; }
    int status;
    if (waitpid(pid, &status, 0) < 0) {
      perror("waitpid() failed");
      failures = WAIT_FAIL;
    } else if (WEXITSTATUS(status) != 0) {
      fprintf(stderr, "a child (pid %d) failed (%d)\n", pid, WEXITSTATUS(status));
      failures = CHILD_FAIL;
    }
  }
  return failures;
}
 
/*
 * The handler for when Ctrl^C is pressed
 */
void handle_sigint(int sig) {
  fail(SIGINT);
}

/*
 * int main()
 *
 * The entry point of the program. Creates shared memory and semaphores, forks
 * children and waits for them to exit.
 */
int main() { 

  // Register a handler for the SIGINT signal (ctrl^c) this allows the program
  // to cleanup semaphores when you control c it.
  struct sigaction act;
  act.sa_handler = handle_sigint;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if(sigaction(SIGINT, &act, 0) == -1)
  {
    perror("sigaction registration failed");
    fail(SIGACTION_FAIL);
  }

  // Initialize variables
  pid_t kids[CHILDREN];
  int failure = 0;
  shmid = -1; // initiallize shmid to sane value.
  semkey = -1;
  seed();

  // create the shared heap and semaphores
  heap_create();
  sem_create();
  attach();
  heap_init(); // TODO: You will have to implement this function in
               // `paralloc.c`
  detach();

  // fork the kids
  if (fork_kids(kids) < 0) {
    // fork failed wait for any fork kids to exit then fail
    waitkids(kids);
    fail(FORK_FAIL);
  }

  // wait for kids
  failure = waitkids(kids);
  if (failure != 0) {
    fail(failure);
  }

  // cleanup when we are done
  cleanup();

  // exit nominally
  return 0;
}

