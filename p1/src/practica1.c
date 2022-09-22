#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 4

void *print_message_function(void *ptr) {
    fprintf(stderr, "Hi! I'm fn\n");
    return NULL;
}
int main(int argc, char *argv[]) {

    int i;
    pthread_t threads[MAX_THREADS];

    for (i = 0; i < MAX_THREADS; i++) {
        pthread_create(&threads[i], NULL, print_message_function, NULL);
    }
    fprintf(stderr, "HI, IM  MAIN\n");
    for (i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}