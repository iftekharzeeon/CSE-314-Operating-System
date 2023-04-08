#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

int item_to_produce, curr_buf_size, curr_buf_size_worker;
int total_items, max_buf_size, num_workers, num_masters;
int item_consumed;

int *buffer;

// Lock and condition variable
pthread_mutex_t lock;
pthread_cond_t filled, empty;

void print_produced(int num, int master) {

  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

  printf("Consumed %d by worker %d\n", num, worker);
  
}


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  while(1) {
    pthread_mutex_lock(&lock);
    if(item_to_produce >= total_items) {
      pthread_cond_broadcast(&filled);
      pthread_cond_broadcast(&empty);
      pthread_mutex_unlock(&lock);
      printf("Master %d done producing\n", thread_id);
      return 0;
    }

    while(curr_buf_size == max_buf_size) {
      pthread_cond_wait(&empty, &lock);
    }
    if(item_to_produce >= total_items) {
      pthread_cond_broadcast(&filled);
      pthread_cond_broadcast(&empty);
      pthread_mutex_unlock(&lock);
      printf("Master %d done producing\n", thread_id);
      return 0;
    }
    buffer[curr_buf_size++] = item_to_produce;
    print_produced(item_to_produce, thread_id);
    item_to_produce++;
    pthread_cond_signal(&filled);
    pthread_mutex_unlock(&lock);
  }
  return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item
void *consume_items(void *data) {
  int thread_id = *((int *)data);

  while(1) {
    pthread_mutex_lock(&lock);
    if (item_consumed == total_items) {
      printf("%d done here consuming\n", thread_id);
      pthread_cond_broadcast(&filled);
      pthread_cond_broadcast(&empty);
      pthread_mutex_unlock(&lock);
      return 0;
    }
    while(curr_buf_size == 0) {
      if (item_consumed == total_items) {
        printf("%d done here consuming\n", thread_id);
        pthread_cond_broadcast(&filled);
        pthread_mutex_unlock(&lock);
        return 0;
      }
      printf("%d thread waiting\n", thread_id);
      pthread_cond_wait(&filled, &lock);
      printf("%d thread done waiting. buff size now %d \n", thread_id, curr_buf_size);
    }
    int consumed = buffer[--curr_buf_size];
    print_consumed(consumed, thread_id);
    item_consumed++;
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&lock);
  }
  return 0;

}

int main(int argc, char *argv[])
{
  int *master_thread_id;
  pthread_t *master_thread;
  item_to_produce = 0;
  curr_buf_size = 0;
  curr_buf_size_worker = 0;

  item_consumed = 0;

  int *worker_thread_id;
  pthread_t *worker_thread;
  
  int i;
  
  if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  } else {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }
    

  buffer = (int *)malloc (sizeof(int) * max_buf_size);

  // Initialize the locks and condition variables
  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&filled, NULL);
  pthread_cond_init(&empty, NULL);

  printf("Started with total_items %d, buf_size: %d, workers: %d, masters: %d\n", total_items, max_buf_size, num_workers, num_masters);

  //create master producer threads
  master_thread_id = (int *)malloc(sizeof(int) * num_masters);
  master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
  for (i = 0; i < num_masters; i++)
    master_thread_id[i] = i;

  for (i = 0; i < num_masters; i++)
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);
  
  //create worker consumer threads
  worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
  worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
  for (i = 0; i < num_workers; i++) {
    worker_thread_id[i] = i;
  }
  for (i = 0; i < num_workers; i++) {
    pthread_create(&worker_thread[i], NULL, consume_items, (void *)&worker_thread_id[i]);
  }

  
  //wait for all threads to complete
  for (i = 0; i < num_masters; i++) {
    pthread_join(master_thread[i], NULL);
    printf("master %d joined\n", i);
  }
  for (i = 0; i < num_workers; i++) {
    pthread_join(worker_thread[i], NULL);
    printf("worker %d joined\n", i);
  }
  
  /*----Deallocating Buffers---------------------*/
  free(buffer);
  free(master_thread_id);
  free(master_thread);
  
  return 0;
}
