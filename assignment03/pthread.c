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

// #define _GNU_SOURCE // to avoid get_cpu() implicit declaration warning

#define NUM_THREADS 128
#define SCHED_POLICY SCHED_FIFO

typedef struct {
    int threadIdx;
} threadParams_t;

// POSIX thread declarations and scheduling attributes
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];

pthread_attr_t fifo_sched_attr;
struct sched_param fifo_param;

void print_scheduler(void) {
    int sched_type = sched_getscheduler(getpid());
    char *sched = malloc(256);
    switch (sched_type)
    {
    case SCHED_FIFO:
        strcpy(sched, "SCHED_FIFO");
        sched = "SCHED_FIFO";
        break;
    case SCHED_OTHER:
        strcpy(sched, "SCHED_OTHER");
        break;
    default:
        strcpy(sched, "UNKNOWN");
        break;
    }

    printf("Pthread policy is: %c\n", sched);
    free(sched);
}

// set up scheduler
void set_scheduler(void) {
    int max_priority; // priority values are 1-99 by default
    int rc;

    printf("Before policy adjustment:\n");
    print_scheduler();

    pthread_attr_init(&fifo_sched_attr);
    pthread_attr_setinheritsched(&fifo_sched_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&fifo_sched_attr, SCHED_POLICY);

    printf("After policy adjustment:\n");
    print_scheduler();

    max_priority = sched_get_priority_max(SCHED_POLICY);
    fifo_param.sched_priority = max_priority;

    pthread_attr_setschedparam(&fifo_sched_attr, &fifo_param);
}

void *counter_thread(void *threadp) {
    int sum = 0, i;
    threadParams_t *threadParams = (threadParams_t *)threadp;

    for(i = 1; i <= (threadParams->threadIdx); i++)
        sum=sum+i;
 
    syslog(LOG_DEBUG, "Thread idx=%d, sum[0...%d]=%d, running on core:%d\n", 
           threadParams->threadIdx,
           threadParams->threadIdx, sum, sched_getcpu());
}

void delay(unsigned int mseconds) {
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

int main (int argc, char *argv[]) {
    // clear syslog
    system("echo > /dev/null | tee /var/log/syslog");
    // create first line
    system("logger [COURSE:1][ASSIGNMENT:3]: `uname -a` | tee /var/log/syslog");
    // open for logging
    openlog("[COURSE:1][ASSIGNMENT:3]", LOG_NDELAY, LOG_DAEMON);

    set_scheduler();
    
    for(int i = 0; i < NUM_THREADS; i++) {
        threadParams[i].threadIdx=i;

        pthread_create(&threads[i],   // pointer to thread descriptor
                      &fifo_sched_attr,     // use fifo
                      counter_thread, // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                     );
   } 

    for(int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
    
    closelog();

    // move syslog output to text file for submission
    // fclose(fopen("assignment2.txt", "w"));
    // system("tail -n 129 /var/log/syslog > assignment2.txt");
    delay(100000);
    system("cp /var/log/syslog assignment3.txt");

    printf("\nComplete\n");
    return 0;
}