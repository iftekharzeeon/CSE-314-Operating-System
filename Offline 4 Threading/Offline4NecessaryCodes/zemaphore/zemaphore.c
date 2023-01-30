#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include "zemaphore.h"

void zem_init(zem_t *s, int value) {
  s->value = value;
  pthread_cond_init(&s->cond_variable, NULL);
  pthread_mutex_init(&s->mutex_lock, NULL);
}

void zem_down(zem_t *s) {
    pthread_mutex_lock(&s->mutex_lock);
    while(s->value <= 0) {
        pthread_cond_wait(&s->cond_variable, &s->mutex_lock);
    }
    s->value--;
    pthread_mutex_unlock(&s->mutex_lock);
}

void zem_up(zem_t *s) {
    pthread_mutex_lock(&s->mutex_lock);
    s->value++;
    pthread_cond_signal(&s->cond_variable);    
    pthread_mutex_unlock(&s->mutex_lock);
}
