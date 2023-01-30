#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  rw->readers = 0;
  rw->waitingReaders = 0;
  rw->waitingWriters = 0;
  rw->writers = 0;
/*  sem_init(&rw->readLock, 0, 1);
  sem_init(&rw->writeLock, 0, 1); */
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
  // Readers will wait on only on active writers. If there are waiting writers, readers will get the preference
  while (rw->writers) {
    pthread_cond_wait(&rw->readLock, &rw->mutexLock);
  }
  rw->waitingReaders--;
  rw->readers++;
  pthread_mutex_unlock(&rw->mutexLock);
}

void ReaderUnlock(struct read_write_lock * rw)
{
  // sem_wait(&rw->readLock);
  // rw->readers--;
  // if (rw->readers == 0) {
  //   sem_post(&rw->writeLock);
  // }
  // sem_post(&rw->readLock);

  pthread_mutex_lock(&rw->mutexLock);
  rw->readers--;
  // If there are no active readers, then signal the writing lock to wake up
  if (rw->readers == 0) {
    pthread_cond_signal(&rw->writeLock);
  }
  pthread_mutex_unlock(&rw->mutexLock);
}

void WriterLock(struct read_write_lock * rw)
{
  // sem_wait(&rw->writeLock);
  pthread_mutex_lock(&rw->mutexLock);
  // Writers will wait on active writers or active readers.
  while (rw->readers || rw->writers) {
    pthread_cond_wait(&rw->writeLock, &rw->mutexLock);
  }
  rw->writers++;
  pthread_mutex_unlock(&rw->mutexLock);
}

void WriterUnlock(struct read_write_lock * rw)
{
  // sem_post(&rw->writeLock);
  pthread_mutex_lock(&rw->mutexLock);
  rw->writers--;
  // If there are waiting readers and writers, prefer the readers first and wake all of them up. If not then wake the writer up
  if (rw->waitingReaders) {
    pthread_cond_broadcast(&rw->readLock);
  } else {
    pthread_cond_signal(&rw->writeLock);
  }
  pthread_mutex_unlock(&rw->mutexLock);
}
