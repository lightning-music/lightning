/**
 * Events are used for inter-thread synchronization
 * Event_wait and Event_timedwait will block the calling
 * thread until another thread calls Event_signal or
 * Event_broadcast
 */
#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

typedef struct Event *Event;

Event
Event_init();

/**
 * Lock the event's mutex
 */
int
Event_lock(Event e);

int
Event_wait(Event e);

int
Event_timedwait(Event e, long ns);

/**
 * Signal the event.
 * This function can block for a long time because
 * it uses Mutex_lock to acquire the Event's mutex
 */
int
Event_signal(Event e, void *value);

/**
 * Signal the event, but use Mutex_trylock
 * to acquire the lock.
 */
int
Event_try_signal(Event e, void *value);

/**
 * Broadcast the event.
 * This function can block for a long time because
 * it uses Mutex_lock to acquire the Event's mutex
 */
int
Event_broadcast(Event e, void *value);

/**
 * Broadcast the event, but use Mutex_trylock
 * to acquire the lock.
 */
int
Event_try_broadcast(Event e, void *value);

void
Event_set_value(Event e, void *value);

void *
Event_value(Event e);

void
Event_free(Event *e);

#endif
