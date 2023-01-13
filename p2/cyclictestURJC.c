#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <time.h>

#define N_CORES (int) sysconf (_SC_NPROCESSORS_ONLN)
#define EXECUTION_TIME 60
#define SLEEP_TIME 1000000
#define BILLION 1000000000L

// Defining debug lines in case to use it.
#ifdef DEBUG
    #define DEBUG_PRINTF(...) printf("DEBUG: "__VA_ARGS__)
#else
    #define DEBUG_PRINTF(...)
#endif

// Controling usage error.
void usage() {
    printf("usage: ./cyclictestURJC\n");
    exit(1);
}

// This thread function should calculate the latency.
void* latency() {
    struct timespec begin, end, time_to_wait, rem;
    long double b_time, e_time;

    time_to_wait.tv_nsec = SLEEP_TIME;
    time_to_wait.tv_sec = 0;
    clock_gettime(CLOCK_MONOTONIC, &begin);
    nanosleep(&time_to_wait, &rem);
    clock_gettime(CLOCK_MONOTONIC, &end);
    b_time = begin.tv_sec + (begin.tv_nsec/BILLION);
    e_time = end.tv_sec + ((double) end.tv_nsec/BILLION);

    printf("INICIO: %Lf\n", b_time);
    printf("FINAL: %f\n", ((double) end.tv_nsec)/BILLION);
    
    return NULL;
}

int main (int argc, char *argv[]) {

    int latency_target_fd, i, n;
    static int32_t latency_target_value;
    pthread_t threads[N_CORES];
    cpu_set_t cpuset[N_CORES];
    struct sched_param param;
    

    if (argc != 1)
        usage();
    
    latency_target_value = 0;
    latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
    if (write(latency_target_fd, &latency_target_value, 4) != 4)
        err(EXIT_FAILURE, "can not read!");

    // Setting affinnity to the different CPUs:
    for (n = 0; n < N_CORES; n++) {
        CPU_ZERO(&cpuset[n]);
        CPU_SET(n, &cpuset[n]);
    }
    
    param.sched_priority = 99;
    for (i = 0; i < 1; i++) {
        if (pthread_create(&threads[i], NULL, latency, NULL) != 0) {
            warnx("error creating thread");
        }
        pthread_setschedparam(threads[i], SCHED_FIFO, &param); 
        if (pthread_setaffinity_np(threads[i], sizeof(cpuset[i]), &cpuset[i]) != 0)
            err(EXIT_FAILURE, "can't set affinity.");
    }
    for (int j = 0; j < 1; j++) {
        pthread_join(threads[j], NULL);
    }
    return EXIT_SUCCESS;
}