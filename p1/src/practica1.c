#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 4

void *thread_function(void *ptr) {
    long double time_before, time_after;
    struct timespec begin;
    char *time_str = malloc(sizeof(begin.tv_sec)*2);
    clock_gettime(CLOCK_REALTIME, &begin);
    sprintf(time_str, "%ld.%ld", begin.tv_sec, begin.tv_nsec);
    char *msg;
    msg = (char *) ptr;
    printf("[%s\n]", time_str);
    time_before = (long double)atof(time_str);
    printf("[%Lf\n]", time_before);
    for (int j = 0; j <= MAX_THREADS; j++) {
        fprintf(stdout, "%s - Iteracion %d: Coste=0.50s\n", msg, j+1);
    }

    free(time_str);
    return NULL;
}


int main(int argc, char *argv[]) {

    int i;
    char *threads_names[] = {"Thread 1", "Thread 2", "Thread 3", "Thread 4"};
    pthread_t threads[MAX_THREADS];

    for (i = 0; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function, (void*) threads_names[i]);
    }
    fprintf(stderr, "HI, IM  MAIN\n");

    for (i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}