#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <syslog.h>
#include <time.h>

// notes for the assignment:
// before printing to syslog clear it with:
// echo > /dev/null | sudo tee /var/log/syslog 

// for the first line you need to print uname -a, use logger command on the command line
// logger [COURSE:X][ASSIGNMENT:Y]: `uname -a`
// the rest of the logging happens through this code

#define NUM_THREADS 1

typedef struct {
    int threadIdx;
} threadParams_t;


// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];


void *hello_world_thread(void *threadp) {
    syslog(LOG_DEBUG, "Hello World from Thread!");
}

void delay(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

int main (int argc, char *argv[]) {
    // clear syslog
    system("echo > /dev/null | tee /var/log/syslog");
    // create first line
    system("logger [COURSE:1][ASSIGNMENT:1]: `uname -a` | tee /var/log/syslog"); 

    openlog("[COURSE:1][ASSIGNMENT:1]", LOG_NDELAY, LOG_DAEMON); 
    for(int i=0; i < NUM_THREADS; i++) {
        threadParams[i].threadIdx=i;

        pthread_create(&threads[i],   // pointer to thread descriptor
                      (void *)0,     // use default attributes
                      hello_world_thread, // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                     );
   }

   syslog(LOG_DEBUG, "Hello World from Main!"); 

   for(int i=0;i<NUM_THREADS;i++)
       pthread_join(threads[i], NULL);
    closelog();

    // copy syslog output to text file for submission
    delay(100000);
    system("cp /var/log/syslog assignment1.txt");

    printf("\nComplete\n");
    return 0;
}
