/**
 * Thread-safe state variable for objects that need to
 * guard against being used in an incomplete state in a
 * separate thread from the one they are initialized in.
 */
#ifndef REALTIME_H_INCLUDED
#define REALTIME_H_INCLUDED

typedef struct Realtime *Realtime;

/**
 * Set state to Initializing
 */
Realtime
Realtime_init();

int
Realtime_set_initializing(Realtime rt);

int
Realtime_set_processing(Realtime rt);

int
Realtime_set_finished(Realtime rt);

int
Realtime_is_initializing(Realtime rt);

int
Realtime_is_processing(Realtime rt);

int
Realtime_is_finished(Realtime rt);

void
Realtime_free(Realtime *rt);

#endif
