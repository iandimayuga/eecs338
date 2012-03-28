/* Ian Dimayuga (icd3)
 * EECS 338 Assignment 4
 * March 28, 2012
 * hw04.c
 */

#include "hw04.h"
#include "cub.h"
#include "staff.h"

int initialT;

enum SEMAPHORES {MUTEX = 0, SURVIVAL, NUM_SEM};

// Keys for Shared memory and semaphores
int shmid = -1;
int semkey = -1;

//process ids
pid_t icub_id[NUM_ICUB];
pid_t sicub_id[NUM_SICUB];
pid_t staff_id[NUM_STAFF];

// Initialize shared memory and semaphores
// Fork all Cub and Staff processes
// Wait
// Cleanup
int main() {
    //seed the random number generator
    seed();

    //set the start time
    initialT = time(NULL);
    
    printout("Simulation starting.");

    //create shared memory segment
    shmid = shmget( IPC_PRIVATE, sizeof(struct daycare), IPC_CREAT | 0666);
    if( shmid < 0) {
        perror( "Error creating shared memory");
        cleanup(EXIT_FAILURE);
    }

    //initialize daycare
    struct daycare* data = shmat( shmid, (void*) 0, 0);
    if( data < 0) {
        perror( "Error attaching shared memory for init");
        cleanup(EXIT_FAILURE);
    }
    data->icub = data->sicub = data->survival = data->istaff = data->sistaff = 0;

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
        int arrive = TIME_ARRIVE + rand() % (TIME_LEAVE + 1 - TIME_ARRIVE);
        int depart = arrive + rand() % (TIME_LEAVE + 1 - arrive);
        icub_id[i] = fork();
        if( icub_id[i] < 0) {
            perror("Error forking icub process " + i);
            cleanup(EXIT_FAILURE);
        } else if( !icub_id[i]) {
            icub( shared, arrive, depart);
        }
    }
    for( i = 0; i < NUM_SICUB; i++) {
        int arrive = TIME_ARRIVE + rand() % (TIME_LEAVE + 1 - TIME_ARRIVE);
        int depart = arrive + rand() % (TIME_LEAVE + 1 - arrive);
        sicub_id[i] = fork();
        if( sicub_id[i] < 0) {
            perror("Error forking sicub process " + i);
            cleanup(EXIT_FAILURE);
        } else if( !sicub_id[i]) {
            sicub( shared, arrive, depart);
        }
    }
    for( i = 0; i < NUM_STAFF; i++) {
        int arrive = TIME_START + rand() % (TIME_ARRIVE + 1 - TIME_START);
        staff_id[i] = fork();
        if( staff_id[i] < 0) {
            perror("Error forking staff process " + i);
            cleanup(EXIT_FAILURE);
        } else if( !staff_id[i]) {
            seed(); //new seed for each member for decision making
            staff( shared, arrive);
        }
    }

    //wait for children
    int status = EXIT_SUCCESS;
    for( i = 0; i < NUM_ICUB + NUM_SICUB + NUM_STAFF; i++) {
        int s;
        wait( &s);
        status = status || WEXITSTATUS(s);
    }

    //mark children as finished
    for( i = 0; i < NUM_ICUB; i++) icub_id[i] = -1;
    for( i = 0; i < NUM_SICUB; i++) sicub_id[i] = -1;
    for( i = 0; i < NUM_STAFF; i++) staff_id[i] = -1;

    //detach shared memory
    if( shmdt( data) < 0) {
        perror( "Error detaching shared memory after init");
        cleanup(EXIT_FAILURE);
    }

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
    counters[SURVIVAL] = 0;
    sem_union.array = counters;

    // set counts as specified
    if( semctl(semkey, 0, SETALL, sem_union) < 0) {
        perror("Error setting semaphore counts");
        cleanup(EXIT_FAILURE);
    }
}

void cleanup( int status) {
    int i;

    //kill all remaining icub processes
    for( i = 0; i < NUM_ICUB; i++) {
        if( icub_id[i] > 0 ) {
            if( kill( icub_id[i], SIGKILL) < 0) {
                fprintf( stderr, "Error killing icub process id %d. Try a manual signal. %s\n",
                    icub_id[i], strerror(errno));
                status = EXIT_FAILURE;
            } else wait(NULL);
        }
    }
    
    //kill all remaining sicub processes
    for( i = 0; i < NUM_SICUB; i++) {
        if( sicub_id[i] > 0 ) {
            if( kill( sicub_id[i], SIGKILL) < 0) {
                fprintf( stderr, "Error killing sicub process id %d. Try a manual signal. %s\n",
                    sicub_id[i], strerror(errno));
                status = EXIT_FAILURE;
            } else wait(NULL);
        }
    }

    //kill all remaining staff processes
    for( i = 0; i < NUM_STAFF; i++) {
        if( staff_id[i] > 0 ) {
            if( kill( staff_id[i], SIGKILL) < 0) {
                fprintf( stderr, "Error killing staff process id %d. Try a manual signal. %s\n",
                    staff_id[i], strerror(errno));
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

    printf("Simulation complete.\n");
    fflush(stdout);

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

void printout( const char* s, ...) {
    int time = simTime();
    pid_t pid = getpid();
    va_list va;
    va_start( va, s);

    char buffer[BUF_LEN];
    vsprintf( buffer, s, va);
    va_end(va);

    printf("%02d00: [%d] %s\n", time, pid, buffer);
    fflush(stdout);
}

void printdata( struct daycare* data) {
    printf("\tICubs: %d\tIStaff: %d\tSICubs: %d\tSIStaff: %d\tSurvival: %d\n\n",
    data->icub, data->istaff, data->sicub, data->sistaff, data->survival);
    fflush(stdout);
}

void semaphore( struct sembuf op, int semkey, char* error) {
    if( semop( semkey, &op, 1) < 0) {
        perror(error);
        _exit(EXIT_FAILURE);
    }
}
