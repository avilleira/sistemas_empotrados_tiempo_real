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
#define EXECUTION_TIME 60000000000L
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

//Transforming timespec structure into a double.
double calculate_time(struct timespec time){
    double new_time;

    new_time = time.tv_sec + ((double) time.tv_nsec/BILLION);
    return new_time;
}
// This thread function should calculate the latency.
void* latency() {

    struct timespec begin, end, time_to_wait, rem, start, now;
    double b_time, e_time, latency;

    time_to_wait.tv_nsec = SLEEP_TIME;
    time_to_wait.tv_sec = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    clock_gettime(CLOCK_MONOTONIC, &begin);
    nanosleep(&time_to_wait, &rem);
    clock_gettime(CLOCK_MONOTONIC, &end);
    // Calculating latency
    b_time = calculate_time(begin);
    e_time = calculate_time(end);
    latency = e_time - b_time;
    printf("INICIO: %.9f\n", b_time);
    printf("FINAL: %ld\n", SLEEP_TIME/BILLION);
    printf("RESTA: %.9f\n", latency);
    
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