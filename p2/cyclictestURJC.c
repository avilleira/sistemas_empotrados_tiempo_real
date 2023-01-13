#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>

#define N_CORES (int) sysconf (_SC_NPROCESSORS_ONLN)

void usage() {
    printf("usage: ./cyclictestURJC\n");
    exit(1);
}

void* thread_f() {
    printf("HOLA soy un thread\n");
    return NULL;
}
int main (int argc, char *argv[]) {

    int latency_target_fd, i;
    pthread_t threads[N_CORES];
    static int32_t latency_target_value;

    if (argc != 1)
        usage();
    
    latency_target_value = 0;
    latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
    if (write(latency_target_fd, &latency_target_value, 4) != 4)
        err(EXIT_FAILURE, "can not read!");
    
    for (i = 0; i < N_CORES; i++) {
        if (pthread_create(&threads[i], NULL, thread_f, NULL) != 0) {
            warnx("error creating thread");
        }
    }
    return EXIT_SUCCESS;
}