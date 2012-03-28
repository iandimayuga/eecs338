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

int main() {
    //set the start time
    initialT = time(NULL);
    int current = 0;
    while( current < TIME_LEAVE) {
        int new = simTime();
        if( new > current) {
            printf("%d\n", new);
            current = new;
        }
    }

    cleanup( EXIT_SUCCESS);
}

int simTime() {
    int current = time(NULL) - initialT;
    current = current / SEC_PER_HOUR;
    return current + TIME_START;
}

void cleanup( int status) {
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
