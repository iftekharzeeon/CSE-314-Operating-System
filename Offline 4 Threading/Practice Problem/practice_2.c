#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>

#define NUM_THREADS 10
pthread_mutex_t mutex;

void *printi(void *arg) {
    // pthread_mutex_lock(&mutex);
    printf("Thread %d\n", (int *) arg);
    // pthread_mutex_unlock(&mutex);
    // pthread_exit(NULL);
}

int main(int argc, char*argv[]) {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);

    int rc;
    while(1) {
        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_mutex_lock(&mutex);
            rc = pthread_create(&threads[i], NULL, printi, (void *)i);
            pthread_mutex_unlock(&mutex);
            if (rc) {
                printf("Error while creating thread, return code from pthread_create is %d\n", rc);
                exit(-1);
            }
        }
    }

    return 0;
}

// gcc -g -pthread main.c -o main