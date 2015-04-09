#ifndef MUTEX_H_INCLUDED
#define MUTEX_H_INCLUDED

typedef struct Mutex *Mutex;

Mutex
Mutex_init();

/**
 * Lock mutex (blocking).
 * Returns 0 on success, nonzero on failure.
 */
int
Mutex_lock(Mutex m);

/**
 * Try to lock mutex (nonblocking).
 * Returns 0 on success, nonzero on failure.
 */
int
Mutex_trylock(Mutex m);

/**
 * Unlock mutex.
 * Returns 0 on success, nonzero on failure.
 */
int
Mutex_unlock(Mutex m);

void
Mutex_free(Mutex *m);

#endif
