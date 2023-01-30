#include <pthread.h>

typedef struct zemaphore {
    int value;
    pthread_cond_t cond_variable;
    pthread_mutex_t mutex_lock;
} zem_t;

void zem_init(zem_t *, int);
void zem_up(zem_t *);
void zem_down(zem_t *);
void debug(zem_t *);
