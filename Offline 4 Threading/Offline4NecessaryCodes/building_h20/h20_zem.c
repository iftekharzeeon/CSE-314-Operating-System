#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"

int total_number_of_hydrogen, total_number_of_oxygen;
int total_number_of_bonds;

int input_number_of_hydrogen;
int input_number_of_oxygen;

int barrier_hydrogen;
int barrier_oxygen;

// Lock and condition variable
zem_t hydro_zem;
zem_t oxy_zem;
zem_t mutex_lock;

void hydrogren(int hydro_num, int current_hydro_num)
{

    printf("A new hydrogen %d came\n\n", hydro_num);
}

void oxygen(int oxy_num, int current_oxy_num)
{

    printf("A new oxygen %d came.\n\n", oxy_num);
}

void *hydrogen_thread_create(void *data)
{
    sleep(rand() % 5);
    int hydrogen_id = *((int *)data);
    zem_down(&mutex_lock);
    barrier_hydrogen++;
    total_number_of_hydrogen++;
    hydrogren(hydrogen_id, barrier_hydrogen);
    if (barrier_hydrogen < 2 || barrier_oxygen < 1)
    {
        printf("Not enough Hydrogen or Oxygen in the barrier. Waiting..\n\n");
        if (!((total_number_of_hydrogen == input_number_of_hydrogen) && (total_number_of_oxygen == input_number_of_oxygen))) {
            zem_up(&mutex_lock); // Releasing the lock
            zem_down(&hydro_zem);
            zem_down(&mutex_lock);
        }
    } else {
        printf("Hydrogen %d passed the barrier.\n\n", hydrogen_id);
    }
    // Make bond
    if (barrier_hydrogen >= 2 && barrier_oxygen >= 1)
    {
        makeBond();
        //Free barrier
        printf("One hydrogen and one oxygen freed from barrier. Barrier: Hydrogen-> %d, Oxygen-> %d\n\n", barrier_hydrogen, barrier_oxygen);
        zem_up(&hydro_zem);
        zem_up(&oxy_zem);
    }
    //Check if complete
    if (total_number_of_hydrogen == input_number_of_hydrogen && total_number_of_oxygen == input_number_of_oxygen) {
        zem_up(&hydro_zem);
        zem_up(&oxy_zem);
    }
    zem_up(&mutex_lock);
}

void *oxygen_thread_create(void *data)
{
    sleep(rand() % 5);
    int oxygen_id = *((int *)data);
    zem_down(&mutex_lock);
    barrier_oxygen++;
    total_number_of_oxygen++;
    oxygen(oxygen_id, total_number_of_oxygen);
    if (barrier_oxygen < 1 || barrier_hydrogen < 2)
    {
        printf("Not enough Hydrogen in the barrier. Waiting..\n\n");
        if (!((total_number_of_hydrogen == input_number_of_hydrogen) && (total_number_of_oxygen == input_number_of_oxygen))) {
            zem_up(&mutex_lock); // Releasing the lock
            zem_down(&oxy_zem);
            zem_down(&mutex_lock);
        }
    } else {
        printf("Oxygen %d passed the barrier\n\n", oxygen_id);
    }
    //Make bond
    if (barrier_hydrogen >= 2 && barrier_oxygen >= 1)
    {
        makeBond();
        
        //Free barrier
        printf("Two hydrogen freed from barrier. Barrier: Hydrogen-> %d, Oxygen-> %d\n\n", barrier_hydrogen, barrier_oxygen);
        for (int i = 0; i < 2; i++) {
            zem_up(&hydro_zem);
        }
    }
    //Check if complete
    if (total_number_of_hydrogen == input_number_of_hydrogen && total_number_of_oxygen == input_number_of_oxygen) {
        zem_up(&hydro_zem);
        zem_up(&oxy_zem);
    }
    zem_up(&mutex_lock);
}

void makeBond()
{
    barrier_hydrogen -= 2;
    barrier_oxygen--;
    total_number_of_bonds++;
    printf("A new H20 bond made. Current number of bonds is %d\n\n", total_number_of_bonds);
}

int main(int argc, char *argv[])
{
    int *hydrogen_thread_id;
    pthread_t *hydrogen_thread;

    int *oxygen_thread_id;
    pthread_t *oxygen_thread;

    int i;

    total_number_of_bonds = 0;
    barrier_hydrogen = 0;
    barrier_oxygen = 0;
    total_number_of_hydrogen = 0;
    total_number_of_oxygen = 0;

    if (argc < 3)
    {
        printf("./h20 #hydrogen_num #oxygen_num ./exe 100 50\n\n");
        exit(1);
    }
    else
    {
        input_number_of_hydrogen = atoi(argv[1]);
        input_number_of_oxygen = atoi(argv[2]);
    }

    // Initialize the locks and condition variables

    zem_init(&hydro_zem, 0);
    zem_init(&oxy_zem, 0);
    zem_init(&mutex_lock, 1);

    printf("Started with Hydrogen %d, Oxygen: %d\n\n", input_number_of_hydrogen, input_number_of_oxygen);

    // create hydrogen oxygen threads
    hydrogen_thread_id = (int *)malloc(sizeof(int) * input_number_of_hydrogen);
    hydrogen_thread = (pthread_t *)malloc(sizeof(pthread_t) * input_number_of_hydrogen);
    for (i = 0; i < input_number_of_hydrogen; i++)
        hydrogen_thread_id[i] = i;

    oxygen_thread_id = (int *)malloc(sizeof(int) * input_number_of_oxygen);
    oxygen_thread = (pthread_t *)malloc(sizeof(pthread_t) * input_number_of_oxygen);
    for (i = 0; i < input_number_of_oxygen; i++)
    {
        oxygen_thread_id[i] = i;
    }

    for (i = 0; i < input_number_of_hydrogen; i++)
        pthread_create(&hydrogen_thread[i], NULL, hydrogen_thread_create, (void *)&hydrogen_thread_id[i]);

    for (i = 0; i < input_number_of_oxygen; i++)
    {
        pthread_create(&oxygen_thread[i], NULL, oxygen_thread_create, (void *)&oxygen_thread_id[i]);
    }

    // wait for all threads to complete
    for (i = 0; i < input_number_of_hydrogen; i++)
    {
        pthread_join(hydrogen_thread[i], NULL);
        printf("Hydrogen %d joined\n\n", i);
    }
    for (i = 0; i < input_number_of_oxygen; i++)
    {
        pthread_join(oxygen_thread[i], NULL);
        printf("Oxygen %d joined\n\n", i);
    }

    /*----Deallocating Buffers---------------------*/
    free(hydrogen_thread_id);
    free(hydrogen_thread);
    free(oxygen_thread_id);
    free(oxygen_thread);

    printf("Total number of hydrogen came: %d, Total number of oxygen came: %d\n\n", total_number_of_hydrogen, total_number_of_oxygen);
    printf("Unused hydrogen: %d, Unused oxygen: %d\n\n", barrier_hydrogen, barrier_oxygen);
    printf("Total number of bonds made %d\n\n", total_number_of_bonds);

    return 0;
}