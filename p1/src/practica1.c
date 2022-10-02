#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 4
#define BILLION 1000000000L

#ifdef DEBUG
    #define DEBUG_PRINTF(...) printf("DEBUG: "__VA_ARGS__)
#else
    #define DEBUG_PRINTF(...)
#endif


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
    struct timespec begin, end;
    double time_before, time_after, cost;
    volatile unsigned long long j;

    msg = (char *) ptr;
    
    for (i = 0; i <= MAX_THREADS; i++) {
        clock_gettime(CLOCK_REALTIME, &begin);
        for (j = 0; j < 400000000ULL; j++);
        time_before = set_time(begin.tv_sec, begin.tv_nsec);
        clock_gettime(CLOCK_REALTIME, &end);
        time_after = set_time(end.tv_sec, end.tv_nsec);
        cost = time_after - time_before;
        fprintf(stdout, "[%ld.%ld] %s - Iteracion %d: Coste=%.2f s.\n",
        begin.tv_sec, begin.tv_nsec,msg, i + 1, cost);
    }
    return NULL;
}


int main(int argc, char *argv[]) {

    int i, j;
    char *threads_names[] = {"Thread 1", "Thread 2", "Thread 3", "Thread 4"};
    pthread_t threads[MAX_THREADS];

    for (i = 0; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function, (void*) threads_names[i]);
    }

    for (j = 0; j < MAX_THREADS; j++) {
        pthread_join(threads[j], NULL);
    }
    return EXIT_SUCCESS;
}