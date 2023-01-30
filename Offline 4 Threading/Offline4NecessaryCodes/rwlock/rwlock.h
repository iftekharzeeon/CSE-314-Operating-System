#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <semaphore.h>

using namespace std;

struct read_write_lock
{
    // Using semaphor
 /*   sem_t writeLock; // No more than one writer can acquire lock
    sem_t readLock; // reader pref-> general; writer pref-> specific readlock */
    
    int readers; // Active reader count
    int writers; // Active writer count, only 1
    int waitingWriters; // Readers in queue
    int waitingReaders; // Writers in queue

    //Using pthread mutex and cv
    pthread_mutex_t mutexLock; // Basic lock for atomicity
    pthread_cond_t readLock; // Condition variable for readers to wait on, writers will signal on it
    pthread_cond_t writeLock; // Condition variable for writers to wait on, readers/writers will signal on it
};

void InitalizeReadWriteLock(struct read_write_lock * rw);
void ReaderLock(struct read_write_lock * rw);
void ReaderUnlock(struct read_write_lock * rw);
void WriterLock(struct read_write_lock * rw);
void WriterUnlock(struct read_write_lock * rw);
