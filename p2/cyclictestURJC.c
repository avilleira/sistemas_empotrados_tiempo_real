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

//Transforming timespec structure into a double.
double calculate_time(struct timespec time){
    double new_time;

    new_time = time.tv_sec + ((double) time.tv_nsec/BILLION);
    return new_time;
}
// This thread function should calculate the latency.
void* latency(void *ptr) {

    struct timespec begin, end, time_to_wait, rem, start, now;
    double b_time, e_time, wait_time;
    long avg, latency, max;
    int thread_id, it_num;

    thread_id = *(int*) ptr;
    time_to_wait.tv_nsec = SLEEP_TIME;
    time_to_wait.tv_sec = 0;
    wait_time = calculate_time(time_to_wait);
    avg = 0;
    it_num = 0;
    max = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    clock_gettime(CLOCK_MONOTONIC, &now);

    while ((calculate_time(now) - calculate_time(start)) < EXECUTION_TIME) {
        it_num++;
        clock_gettime(CLOCK_MONOTONIC, &begin);
        nanosleep(&time_to_wait, &rem);
        clock_gettime(CLOCK_MONOTONIC, &end);
        // Calculating latency
        b_time = calculate_time(begin);
        e_time = calculate_time(end);
        latency = (long) ((e_time - b_time - wait_time)*BILLION);
        avg = avg + latency;

        if (latency >= max)
            max = latency;
        clock_gettime(CLOCK_MONOTONIC, &now);
    }
    //printing final result.
    printf("[%d] latencia media = %09ld ns. | max = %09ld ns.\n", 
        thread_id, avg/it_num, max);
    
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
    for (i = 0; i < N_CORES; i++) {
        if (pthread_create(&threads[i], NULL, latency, (void*) &i) != 0) {
            warnx("error creating thread");
        }
        pthread_setschedparam(threads[i], SCHED_FIFO, &param); 
        if (pthread_setaffinity_np(threads[i], sizeof(cpuset[i]), &cpuset[i]) != 0)
            err(EXIT_FAILURE, "can't set affinity.");
    }
    for (int j = 0; j < N_CORES; j++) {
        pthread_join(threads[j], NULL);
    }
    return EXIT_SUCCESS;
}