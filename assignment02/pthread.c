#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <syslog.h>

// notes for the assignment:
// before printing to syslog clear it with:
// echo > /dev/null | sudo tee /var/log/syslog 

// for the first line you need to print uname -a, use logger command on the command line
// logger [COURSE:X][ASSIGNMENT:Y]: `uname -a`
// the rest of the logging happens through this code

#define NUM_THREADS 128

typedef struct {
    int threadIdx;
} threadParams_t;


// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];


void *counter_thread(void *threadp) {
    int sum = 0, i;
    threadParams_t *threadParams = (threadParams_t *)threadp;

    for(i = 1; i <= (threadParams->threadIdx); i++)
        sum=sum+i;
 
    syslog(LOG_DEBUG, "Thread idx=%d, sum[0...%d]=%d\n", 
           threadParams->threadIdx,
           threadParams->threadIdx, sum);
}


int main (int argc, char *argv[]) {
    system("echo > /dev/null | sudo tee /var/log/syslog");
    system("logger [COURSE:1][ASSIGNMENT:2]: uname -a | tee /var/log/syslog"); 
    openlog("[COURSE:1][ASSIGNMENT:2]", LOG_NDELAY, LOG_DAEMON); 
    for(int i = 0; i < NUM_THREADS; i++) {
        threadParams[i].threadIdx=i;

        pthread_create(&threads[i],   // pointer to thread descriptor
                      (void *)0,     // use default attributes
                      counter_thread, // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                     );
   } 

   for(int i = 0; i < NUM_THREADS; i++)
       pthread_join(threads[i], NULL);
    closelog();

    printf("Complete\n");
    system("tail -f /var/log/syslog > assignment2.txt");
    return 0;
}