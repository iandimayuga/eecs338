/* Ian Dimayuga (icd3)
 * EECS 338 Assignment 4
 * March 28, 2012
 * staff.c
 */

#include "staff.h"

void staff( struct shared_info info, int arrive) {
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

    //wait(mutex): CRITICAL SECTION BEGIN
    semaphore( wait_mutex, info.semkey, "wait(mutex)");

    //arrive
    data->survival++;
    printout("Staff has arrived in Survival Hall.");
    printdata(data);

    //signal(mutex): CRITICAL SECTION END
    semaphore( signal_mutex, info.semkey, "signal(mutex)");

    while( simTime() < TIME_LEAVE) {

        //wait(survival)
        semaphore( wait_survival, info.semkey, "wait(survival)");

        if( simTime() < TIME_LEAVE) {
            bool working = true;
            //Time to work!
            while( working) {
                // Check endurance while busywaiting
                while( rand() % ENDURANCE > 0 && simTime() < TIME_LEAVE) {
                    sleep(1);
                }

                if( simTime() < TIME_LEAVE) {
                    //Need a break. Call colleague

                    //wait(mutex): CRITICAL SECTION BEGIN
                    semaphore( wait_mutex, info.semkey, "wait(mutex)");

                    if( data->survival > 0) {
                        //signal(survival)
                        semaphore( signal_survival, info.semkey, "signal(survival)");
                        working = false;

                        printout("Staff swapping out.");
                        printdata(data);

                    } else {
                        working = true; //can't go on break!
                    }

                    //signal(mutex): CRITICAL SECTION END
                    semaphore( signal_mutex, info.semkey, "signal(mutex)");
                } else {
                    //Day is over. Head back to Survival.
                    //wait(mutex): CRITICAL SECTION BEGIN
                    semaphore( wait_mutex, info.semkey, "wait(mutex)");

                    if(working) data->survival++;
                    if( data->istaff > 0) data->istaff--;
                    else if( data->sistaff > 0) data->sistaff--;

                    printout("Day is over. Staff returning to Hall..");
                    printdata(data);

                    //signal(mutex): CRITICAL SECTION END
                    semaphore( signal_mutex, info.semkey, "signal(mutex)");
                    working = false;
                }
            }
        }
    }
    
    
    //wait(mutex): CRITICAL SECTION BEGIN
    semaphore( wait_mutex, info.semkey, "wait(mutex)");

    if(data->survival > 0) {
        //make sure the rest of the staff know to leave
        semaphore( signal_survival, info.semkey, "signal(survival)");
        data->survival--;
        printout("Staff leaving for the day.");
        printdata(data);
    }

    //signal(mutex): CRITICAL SECTION END
    semaphore( signal_mutex, info.semkey, "signal(mutex)");

    _exit(EXIT_SUCCESS);
}
