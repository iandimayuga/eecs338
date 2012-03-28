/* Ian Dimayuga (icd3)
 * EECS 338 Assignment 4
 * March 28, 2012
 * hw04.h
 */

#ifndef HW04_H
#define HW04_H

//number of seconds in simulation to correspond to one daycare hour
#define SEC_PER_HOUR 2

//schedule in hours
#define TIME_START 5
#define TIME_ARRIVE 8
#define TIME_LEAVE 18

//maximum number
#define NUM_ICUB = 5
#define NUM_SICUB = 7
#define NUM_STAFF = 5

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
struct daycare_data {
    int icub_count; //number of ICubs
    int sicub_count; //number of SICubs
    int survival_count; //number of staff in Survival
    int istaff_count; //number of staff in ICub room
    int sistaff_count; //number of staff in SICub room
};

//will give simulation time in hours
int simTime();

//clean up and exit
void cleanup( int status);

#endif //HW04_H
