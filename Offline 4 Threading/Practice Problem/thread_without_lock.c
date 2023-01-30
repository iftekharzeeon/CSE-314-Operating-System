#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10

static volatile int counter;

void *increaseCounter() {
    // printf("%s: begin\n", (char *) arg);
    for (int i = 0; i < 100000; i++) {
        counter++;
    }
    // printf("%s: done\n", (char *) arg);
}

int main(int argc, char*argv[]) {
    // pthread_t threads[NUM_THREADS];
    pthread_t p1, p2;
    counter = 0;

    pthread_create(&p1, NULL, increaseCounter, NULL);
    pthread_create(&p2, NULL, increaseCounter, NULL);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);


    // int rc;
    // for (int i = 0; i < NUM_THREADS; i++) {
    //     rc = pthread_create(&threads[i], NULL, increaseCounter, "A");
    //     if (rc) {
    //         printf("Error while creating thread, return code from pthread_create is %d\n", rc);
    //         exit(-1);
    //     }
    // }

    // for (int i = 0; i < NUM_THREADS; i++) {
    //     pthread_join(threads[i], NULL);
    // }
    printf("Counter value %d\n", counter);
    return 0;
}