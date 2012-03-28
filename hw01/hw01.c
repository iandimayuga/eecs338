/* Ian Dimayuga (icd3)
 * EECS 338 Assignment 1
 * February 6, 2012
 * hw01.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "hw01.h"

int main( int argc, char* argv[]) {
    char* hostname = calloc( 1, LEN_HOST);

    //get the hostname of this process
    if( gethostname( hostname, LEN_HOST) < 0) {
        perror("Error in function gethostname()");
        exit(-1);
    }

    //group id
    gid_t gid = getgid();
    //user id
    uid_t uid = getuid();
    //process id
    pid_t pid = getpid();

    printf("Process Information:\nHostname: %s\nGroup ID: %u\nUser ID: %u\nProcess ID: %u\n",
            hostname, gid, uid, pid);
    fflush(stdout);

    //initialize counter
    int count = 6;
    printf("\nStarting countdown...\n");
    fflush(stdout);

    int fid; //fork id

    int i;
    //child spawning loop (5...1)
    for( i = 5; i > 0; i--) {
        fid = fork();

        if( fid < 0) {
            perror("Error forking in countdown");
            exit(-1);
        } else if( fid > 0) { //parent process
            wait( fid);
        } else { //child process
            //decrement counter variable
            count--;

            //build output string
            char* outstr = calloc( 1, LEN_BUF);
            snprintf( outstr, 32, "Index = %d, Counter = %d", i, count);

            //execute echo with output string
            if( execl(ECHO_PATH, "echo", outstr, (char*)NULL ) < 0) {
                perror("Error in function exec()");
                exit(-1);
            }
        }
    }

    printf("\n");
    fflush(stdout);

    //fork child one
    int fid1 = fork();
    if( fid1 < 0) {
        perror("Error forking child one");
        exit(-1);
    } else if( fid1 == 0) { //child process
        childOne();
        _exit(0);
    }

    //fork child two
    int fid2 = fork();
    if( fid2 < 0) {
        perror("Error forking child two");
        exit(-1);
    } else if( fid2 == 0) { //child process
        childTwo();
        _exit(0);
    }
    
    wait(fid1);
    wait(fid2);
    
    printf("\nParent finished.\n");
    fflush(stdout);

    exit(0);
}

void childOne() {
    pid_t pid = getpid();
    sleep(1);
    printf("Child %u: There once was a man from Vietnam\n", pid);
    fflush(stdout);
    sleep(SLEEPYTIME);
    printf("Child %u: He said, \"It's no use,\n", pid);
    fflush(stdout);
    sleep(SLEEPYTIME);
    printf("Child %u: This darn ':(){ :|:& };:' fork bomb!\"\n", pid);
    fflush(stdout);
}

void childTwo() {
    pid_t pid = getpid();
    sleep(SLEEPYTIME);
    printf("Child %u: Who asked his OS to stay calm.\n", pid);
    fflush(stdout);
    sleep(SLEEPYTIME);
    printf("Child %u: But I cannot defuse\n", pid);
    fflush(stdout);
}
