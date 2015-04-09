#include <assert.h>
#include <pthread.h>
#include <stddef.h>

#include "mem.h"
#include "mutex.h"

struct Mutex {
    pthread_mutex_t mutex;
};

Mutex
Mutex_init() {
    Mutex m;
    NEW(m);
    pthread_mutex_init(&m->mutex, NULL);
    return m;
}

int
Mutex_lock(Mutex m) {
    assert(m);
    return pthread_mutex_lock(&m->mutex);
}

int
Mutex_trylock(Mutex m) {
    assert(m);
    return pthread_mutex_trylock(&m->mutex);
}

int
Mutex_unlock(Mutex m) {
    assert(m);
    return pthread_mutex_unlock(&m->mutex);
}

void
Mutex_free(Mutex *m) {
    assert(m && *m);
    pthread_mutex_destroy(&(*m)->mutex);
    FREE(*m);
}
