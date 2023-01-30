#include <pthread.h>
#include <stdio.h>

#define NUM_THREADS 5

void *printHello(void *threadId) {
    int tId;
    tId = (int) threadId;
    printf("Hello World! It's me, thread: %d!\n", tId);
    pthread_exit(NULL);
}

int main(int argc, char*argv[]) {
    pthread_t threads[NUM_THREADS];

    int rc;
    for (int i = 0; i < NUM_THREADS; i++) {
        printf("In Main: creating thread %d\n", i);
        rc = pthread_create(&threads[i], NULL, printHello, (void *)i);
        if (rc) {
            printf("Error while creating thread, return code from pthread_create is %d\n", rc);
            exit(-1);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}