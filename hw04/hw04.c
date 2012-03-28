/* Ian Dimayuga (icd3)
 * EECS 338 Assignment 4
 * March 28, 2012
 * hw04.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "hw04.h"
#include "cub.h"
#include "staff.h"

int initialT;

enum SEMAPHORES {MUTEX = 0, SURVIVAL, NUM_SEM};

// Keys for Shared memory and semaphores
int shmid = -1;
int semkey = -1;

//process ids
pid_t[] icub_id  = malloc( NUM_ICUB * sizeof( pid_t));
pid_t[] sicub_id = malloc( NUM_SICUB * sizeof( pid_t));
pid_t[] staff_id = malloc( NUM_STAFF * sizeof( pid_t));

// Initialize shared memory and semaphores
// Fork all Cub and Staff processes
// Wait
// Cleanup
int main() {
    //seed the random number generator
    seed();
    
    //set the start time
    initialT = time(NULL);

    //create shared memory segment
    shmid = shmget( IPC_PRIVATE, sizeof(struct daycare_data), IPC_CREAT | 0666);
    if( shmid < 0) {
        perror( "Error creating shared memory");
        cleanup(EXIT_FAILURE);
    }

    //create semaphore group
    semkey = semget(IPC_PRIVATE, NUM_SEM, IPC_CREAT | 0666);
    if ( semkey < 0) {
        perror("Error getting semaphores");
        cleanup(EXIT_FAILURE);
    }
    initCounts( semkey);

    //shared data information
    struct shared_info shared = {
        shmid,
        semkey,
        MUTEX,
        SURVIVAL
    };

    //fork children
    int i;
    for( i = 0; i < NUM_ICUB; i++) {
        icub_id[i] = fork();
        if( icub_id[i] < 0) {
            perror("Error forking icub process " + i);
            cleanup(EXIT_FAILURE);
        } else if( !icub_id[i]) {
            icub( shared);
        }
    }
    for( i = 0; i < NUM_SICUB; i++) {
        sicub_id[i] = fork();
        if( sicub_id[i] < 0) {
            perror("Error forking sicub process " + i);
            cleanup(EXIT_FAILURE);
        } else if( !sicub_id[i]) {
            sicub( shared);
        }
    }
    for( i = 0; i < NUM_STAFF; i++) {
        staff_id[i] = fork();
        if( staff_id[i] < 0) {
            perror("Error forking staff process " + i);
            cleanup(EXIT_FAILURE);
        } else if( !staff_id[i]) {
            staff( shared);
        }
    }

    //wait for children
    int status = EXIT_SUCCESS;
    for( i = 0; i < NUM_ICUB + NUM_SICUB + NUM_STAFF; i++) {
        int s;
        wait( &s);
        status ||= s;
    }

    //mark children as finished
    for( i = 0; i < NUM_ICUB; i++) icub_id[i] = -1;
    for( i = 0; i < NUM_SICUB; i++) sicub_id[i] = -1;
    for( i = 0; i < NUM_STAFF; i++) staff_id[i] = -1;

    cleanup( status);

    return status;
}

int simTime() {
    int current = time(NULL) - initialT;
    current = current / SEC_PER_HOUR;
    return current + TIME_START;
}

void initCounts( int semkey) {
    // specify semaphore counts
    union semun sem_union;
    unsigned short counters[2];
    counters[MUTEX] = 1;
    counters[SURVIVAL] = NUM_STAFF;
    sem_union.array = counters;

    // set counts as specified
    if( semctl(semkey, 0, SETALL, sem_union) < 0);
        perror("Error setting semaphore counts");
        cleanup(EXIT_FAILURE);
    }
}

void cleanup( int status) {
    int i;

    //kill all remaining icub processes
    for( i = 0; i < NUM_ICUB; i++) {
        if( icub_id[i] > 0 ) {
            if( kill( staff_id[i], SIGKILL) < 0);
                perror("Error killing icub process id " + staff_id[i] + ". Try a manual signal.");
                status = EXIT_FAILURE;
            } else wait(NULL);
        }
    }
    
    //kill all remaining sicub processes
    for( i = 0; i < NUM_SICUB; i++) {
        if( sicub_id[i] > 0 ) {
            if( kill( staff_id[i], SIGKILL) < 0);
                perror("Error killing sicub process id " + staff_id[i] + ". Try a manual signal.");
                status = EXIT_FAILURE;
            } else wait(NULL);
        }
    }

    //kill all remaining staff processes
    for( i = 0; i < NUM_STAFF; i++) {
        if( staff_id[i] > 0 ) {
            if( kill( staff_id[i], SIGKILL) < 0);
                perror("Error killing staff process id " + staff_id[i] + ". Try a manual signal.");
                status = EXIT_FAILURE;
            } else wait(NULL);
        }
    }

    // remove semaphore group
    if (semkey >= 0) {
        if (semctl(semkey, 0, IPC_RMID) < 0) {
            perror("Error removing semaphores");
            status = EXIT_FAILURE;
        }
    }

    // remove shared memory segment
    if (shmid >= 0) {
        if (shmctl(shmid, IPC_RMID, (struct shmid_ds *) NULL)) {
            perror("Error removing shared memory");
            status = EXIT_FAILURE;
        }
    }

    exit( status);
}

void seed() {
    int urandfd;
    char randdata[sizeof(int)];
    if ((urandfd = open("/dev/urandom", O_RDONLY)) <  0) {
        perror("could not open /dev/urandom");
        cleanup( EXIT_FAILURE);
    }
    if (read(urandfd, randdata, sizeof(randdata)) < sizeof(randdata)) {
        perror("could not read all the data from /dev/urandom");
        cleanup( EXIT_FAILURE);
    }
    if (close(urandfd) < 0) {
        perror("closing /dev/urandom failed");
        cleanup( EXIT_FAILURE);
    }
    srand(*((int *)randdata));
}
