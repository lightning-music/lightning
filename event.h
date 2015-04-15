/**
 * Events are used for inter-thread synchronization
 * Event_wait and Event_timedwait will block the calling
 * thread until another thread calls Event_signal or
 * Event_broadcast
 */
#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

typedef struct LightningEvent *LightningEvent;

LightningEvent
LightningEvent_init();

/**
 * Lock the event's mutex
 */
int
LightningEvent_lock(LightningEvent e);

int
LightningEvent_wait(LightningEvent e);

int
LightningEvent_timedwait(LightningEvent e, long ns);

/**
 * Signal the event.
 * This function can block for a long time because
 * it uses Mutex_lock to acquire the LightningEvent's mutex
 */
int
LightningEvent_signal(LightningEvent e, void *value);

/**
 * Signal the event, but use Mutex_trylock
 * to acquire the lock.
 */
int
LightningEvent_try_signal(LightningEvent e, void *value);

/**
 * Broadcast the event.
 * This function can block for a long time because
 * it uses Mutex_lock to acquire the LightningEvent's mutex
 */
int
LightningEvent_broadcast(LightningEvent e, void *value);

/**
 * Broadcast the event, but use Mutex_trylock
 * to acquire the lock.
 */
int
LightningEvent_try_broadcast(LightningEvent e, void *value);

void
LightningEvent_set_value(LightningEvent e, void *value);

void *
LightningEvent_value(LightningEvent e);

void
LightningEvent_free(LightningEvent *e);

#endif
