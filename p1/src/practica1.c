#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <err.h>
#include <unistd.h>

#define MAX_THREADS 4
#define BILLION 1000000000L
#define PERIOD 0.90

#ifdef DEBUG
    #define DEBUG_PRINTF(...) printf("DEBUG: "__VA_ARGS__)
#else
    #define DEBUG_PRINTF(...)
#endif

void usage() {
    fprintf(stderr, "usage: practica1 \n");
    exit(1);
}

double set_time(double sec, long nsec) {
    long double time;

    time = sec + ((double) nsec / BILLION);
    //Debug line used to watch if the time operation was done right.
    DEBUG_PRINTF("[[[%Lf]]]]\n", time);
    return time;
}

void *thread_function(void *ptr) {
    int i;
    char *msg;
    struct timespec begin, end, time_to_wait, remaining;
    double time_before, time_after, cost;
    volatile unsigned long long j;

    msg = (char *) ptr;
    
    for (i = 0; i <= MAX_THREADS; i++) {
        clock_gettime(CLOCK_REALTIME, &begin); //Get the time before the loop.
        for (j = 0; j < 400000000ULL; j++);
        time_before = set_time(begin.tv_sec, begin.tv_nsec);
        clock_gettime(CLOCK_REALTIME, &end);
        time_after = set_time(end.tv_sec, end.tv_nsec);
        cost = time_after - time_before;
        time_to_wait.tv_nsec = (0.90 - cost) * BILLION;
        time_to_wait.tv_sec = 0;
        
        if ( cost >= PERIOD) {
            fprintf(stderr, "[%ld.%ld] %s - Iteracion %d: Coste=%.2f s. (fallo temporal)\n", 
            begin.tv_sec, begin.tv_nsec,msg, i + 1, cost);
        }
        else {
            fprintf(stdout, "[%ld.%ld] %s - Iteracion %d: Coste=%.2f s.\n",
            begin.tv_sec, begin.tv_nsec,msg, i + 1, cost);
            /*nanosleep() suspends  the execution of the calling thread for a 
            time specified in time_to_wait.*/
            nanosleep(&time_to_wait, &remaining);
        }
    }
    return NULL;
}


int main(int argc, char *argv[]) {

    int i, j;
    char *threads_names[] = {"Thread 1", "Thread 2", "Thread 3", "Thread 4"};
    pthread_t threads[MAX_THREADS];

    if (argc != 1) {
        usage();
    }

    for (i = 0; i < MAX_THREADS; i++) {
        if ( pthread_create(&threads[i], NULL, thread_function,
         (void*) threads_names[i]) != 0) {
            warnx("error creating thread"); 
        }
    }

    for (j = 0; j < MAX_THREADS; j++) {
        if ( pthread_join(threads[j], NULL) != 0) {
            warnx("error joining thread");
        }
    }

    return EXIT_SUCCESS;
}