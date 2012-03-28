/* Ian Dimayuga (icd3)
 * EECS 338 Assignment 4
 * March 28, 2012
 * staff.c
 */

#include "staff.h"

void staff( struct shared_info info, int arrive, int id) {
    //semaphore instructions
    struct sembuf wait_mutex = {info.mutex, WAIT, 0};
    struct sembuf signal_mutex = {info.mutex, SIGNAL, 0};
    struct sembuf wait_survival = {info.survival, WAIT, 0};
    struct sembuf signal_survival = {info.survival, SIGNAL, 0};

    //grab the shared memory
    struct daycare* data = shmat( info.shmid, (void*) 0, 0);
    if( data < 0) {
        perror( "Error attaching shared memory");
        _exit(EXIT_FAILURE);
    }

    //wait until the actual arrival time
    while( simTime() < arrive);

    while( simTime() < TIME_LEAVE) {

        //wait(mutex): CRITICAL SECTION BEGIN
        semaphore( wait_mutex, info.semkey, "wait(mutex)");

        //arrive
        data->survival++;
        data->staff[id] = HALL;
        printout("Staff has arrived in Survival Hall. Count: %d.", data->survival);

        //signal(mutex): CRITICAL SECTION END
        semaphore( signal_mutex, info.semkey, "signal(mutex)");

        //wait(survival)
        semaphore( wait_survival, info.semkey, "wait(survival)");

        //Time to work! Check endurance while busywaiting
        while( rand() % ENDURANCE > 0 && simTime() < TIME_LEAVE);

        if( simTime() < TIME_LEAVE) {
            //Need a break. Call colleague

            //wait(mutex): CRITICAL SECTION BEGIN
            semaphore( wait_mutex, info.semkey, "wait(mutex)");

            if( data->survival > 0) {

            }

            //signal(mutex): CRITICAL SECTION END
            semaphore( signal_mutex, info.semkey, "signal(mutex)");
        }
    }
    _exit(EXIT_SUCCESS);
}
