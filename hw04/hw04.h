/* Ian Dimayuga (icd3)
 * EECS 338 Assignment 4
 * March 28, 2012
 * hw04.h
 */

#ifndef HW04_H
#define HW04_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

//semoperations
#define WAIT -1
#define SIGNAL 1

//number of seconds in simulation to correspond to one daycare hour
#define SEC_PER_HOUR 2

//schedule in hours
#define TIME_START 5
#define TIME_ARRIVE 8
#define TIME_LEAVE 18

//maximum number
#define NUM_ICUB 5
#define NUM_SICUB 7
#define NUM_STAFF 5

//required staff
#define ICUB_PER_STAFF 3
#define SICUB_PER_STAFF 4

//staff endurance (reciprocal of tiredness probability)
#define ENDURANCE 10

//maximum char buffer
#define BUF_LEN 1024

//union semun copied from semctl man page
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

// information about shared data
struct shared_info {
    int shmid; // shared memory ID
    int semkey; // semaphore group key
    int mutex; // index for mutex semaphore
    int survival; // index for survival semaphore
};

// shared state data
struct daycare {
    int icub; // number of ICubs
    int sicub; // number of SICubs
    int survival; // number of staff in Survival
    int istaff; // number of staff in ICub room
    int sistaff; // number of staff in SICub room
};

// gives simulation time in hours
int simTime();

// initialize semaphore counts
void initCounts( int semkey);

// clean up and exit
void cleanup( int status);

// seed rand
void seed();

//print with pid and timestamp
void printout( const char* s, ...);

//print the room counts
void printdata( struct daycare* data);

//semaphore operation for processes to call
//error is for error message in case of failure
void semaphore( struct sembuf op, int semkey, char* error);

#endif //HW04_H
