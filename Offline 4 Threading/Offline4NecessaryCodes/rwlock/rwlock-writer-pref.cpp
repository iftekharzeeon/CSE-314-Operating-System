#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  rw->readers = 0;
  rw->writers = 0;
  rw->waitingWriters = 0;
  rw->waitingReaders = 0;
  // sem_init(&rw->writeLock, 0, 1);
  // sem_init(&rw->readLock, 0, 1);
  pthread_mutex_init(&rw->mutexLock, NULL);
  pthread_cond_init(&rw->readLock, NULL);
  pthread_cond_init(&rw->writeLock, NULL);

}

void ReaderLock(struct read_write_lock * rw)
{
  // sem_wait(&rw->readLock);
  // rw->readers++;
  // if (rw->readers == 1) {
  //   sem_wait(&rw->writeLock);
  // }
  // sem_post(&rw->readLock);

  pthread_mutex_lock(&rw->mutexLock);
  rw->waitingReaders++;
  // Since writers are preferred, if there are waiting writers or active writers, readers will wait.
  while (rw->writers || rw->waitingWriters) {
    pthread_cond_wait(&rw->readLock, &rw->mutexLock);
  }
  rw->readers++;
  rw->waitingReaders--;
  pthread_mutex_unlock(&rw->mutexLock);
}

void ReaderUnlock(struct read_write_lock * rw)
{
  // rw->readers--;
  // if (rw->readers == 0) {
  //   sem_post(&rw->writeLock);
  // }
  pthread_mutex_lock(&rw->mutexLock);
  rw->readers--;
  // If there are no active readers and waiting writers, then signal the writing lock to wake up
  if (rw->readers == 0 && rw->waitingWriters) {
    pthread_cond_signal(&rw->writeLock);
  }
  pthread_mutex_unlock(&rw->mutexLock);
}

void WriterLock(struct read_write_lock * rw)
{
  // rw->waitingWriters++;
  // if (rw->readers) {
  //   sem_wait(&rw->readLock);
  // }
  // sem_wait(&rw->writeLock);
  // sem_post(&rw->readLock);
  // rw->waitingWriters--;
  pthread_mutex_lock(&rw->mutexLock);
  rw->waitingWriters++;
  // Writers will wait on active writers or active readers.
  while (rw->readers || rw->writers) {
    pthread_cond_wait(&rw->writeLock, &rw->mutexLock);
  }
  rw->waitingWriters--;
  rw->writers++;
  pthread_mutex_unlock(&rw->mutexLock);
}

void WriterUnlock(struct read_write_lock * rw)
{
  // sem_post(&rw->writeLock);
  pthread_mutex_lock(&rw->mutexLock);
  rw->writers--;
  // If there are waiting readers and writers, prefer the writers first and wake the writer up. 
  // If not then wake all the waiting readers up
  if (rw->waitingWriters) {
    pthread_cond_signal(&rw->writeLock);
  } else if (rw->waitingReaders) {
    pthread_cond_broadcast(&rw->readLock);
  }
  pthread_mutex_unlock(&rw->mutexLock);
}
