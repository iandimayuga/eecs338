/* Ian Dimayuga (icd3)
 * EECS 338 Assignment 4
 * March 28, 2012
 * cub.c
 */

#include "cub.h"

void icub( struct shared_info info, int arrive, int depart) {
    cub( info, arrive, depart, true);
}

void sicub( struct shared_info info, int arrive, int depart) {
    cub( info, arrive, depart, false);
}

void cub( struct shared_info info, int arrive, int depart, bool icub) {
    // child will not come to daycare today
    if( arrive >= depart) {
        printout("This %s will not arrive today.", icub ? "ICub" : "SICub");
        _exit(EXIT_SUCCESS);
    }

    //semaphore instructions
    struct sembuf wait_mutex = {info.mutex, WAIT, 0};
    struct sembuf signal_mutex = {info.mutex, SIGNAL, 0};
    struct sembuf signal_survival = {info.survival, SIGNAL, 0};

    //grab the shared memory
    struct daycare* data = shmat( info.shmid, (void*) 0, 0);
    if( data < 0) {
        perror( "Error attaching shared memory");
        _exit(EXIT_FAILURE);
    }

    //wait until the actual arrival time
    while( simTime() < arrive);

    //wait(mutex): CRITICAL SECTION BEGIN
    semaphore( wait_mutex, info.semkey, "wait(mutex)");

    //increment cub count
    if( icub) data->icub++;
    else data->sicub++;

    printout("%s has arrived. Room count: %d. Staff in room: %d.", 
          icub ? "ICub" : "SICub", 
          icub ? data->icub : data->sicub, 
          icub ? data->istaff : data->sistaff);

    //detect new staff need
    if( icub ?
      (data->icub > data->istaff * ICUB_PER_STAFF) :
      (data->sicub > data->sistaff * SICUB_PER_STAFF)) {

        printout("Staff needed in %s room!", icub ? "ICub" : "SICub");

        //signal(survival)
        semaphore( signal_survival, info.semkey, "signal(survival)");

        //move staff to this room
        data->survival--;
        if( icub) data->istaff++;
        else data->sistaff++;

        printout("Staff has entered %s room. Staff in room: %d. Staff in survival: %d.", 
              icub ? "ICub" : "SICub", 
              icub ? data->istaff : data->sistaff,
              data->survival);
    }

    //signal(mutex): CRITICAL SECTION END
    semaphore( signal_mutex, info.semkey, "signal(mutex)");

    //time to play
    while( simTime() < depart);

    //wait(mutex)
    semaphore( wait_mutex, info.semkey, "wait(mutex)");

    //leave the room
    if( icub) data->icub--;
    else data->sicub--;

    printout("%s has departed. Room count: %d. Staff in room: %d.", 
          icub ? "ICub" : "SICub", 
          icub ? data->icub : data->sicub, 
          icub ? data->istaff : data->sistaff);

    //signal(mutex)
    semaphore( signal_mutex, info.semkey, "signal(mutex)");

    //go home
    _exit(EXIT_SUCCESS);
}
