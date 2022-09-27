#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 4

void *print_message_function(void *ptr) {
    char *msg;
    msg = (char *) ptr;
    for (int j = 0; j <= MAX_THREADS; j++) {
        fprintf(stdout, "%s - Iteracion %d: Coste=0.50s\n", msg, j+1);
    }
    return NULL;
}


int main(int argc, char *argv[]) {

    int i;
    char **threads_names = {"Thread 1", "Thread 2", "Thread 3", "Thread 4"};
    pthread_t threads[MAX_THREADS];

    for (i = 0; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, print_message_function, (void*) threads_names[i]);
    }
    fprintf(stderr, "HI, IM  MAIN\n");

    for (i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}