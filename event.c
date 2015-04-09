#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <time.h>

#include "event.h"
#include "mem.h"

typedef enum {
    EventState_NotReady,
    EventState_Ready
} EventState;

struct Event {
    void *val;
    EventState state;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

Event
Event_init()
{
    Event e;
    NEW(e);
    pthread_mutex_init(&e->mutex, NULL);
    pthread_cond_init(&e->cond, NULL);
    e->state = EventState_NotReady;
    e->val = NULL;
    return e;
}

int
Event_lock(Event e)
{
    assert(e);
    return pthread_mutex_lock(&e->mutex);
}

int
Event_wait(Event e)
{
    assert(e);
    e->state = EventState_NotReady;
    int result = pthread_cond_wait(&e->cond, &e->mutex);
    if (e->state != EventState_Ready) {
        return Event_wait(e);
    } else {
        return result;
    }
}

int
Event_timedwait(Event e, long ns)
{
    assert(e);
    e->state = EventState_NotReady;
    struct timespec time;
    time.tv_nsec = ns;
    int result = pthread_cond_timedwait(&e->cond, &e->mutex, &time);
    if (e->state != EventState_Ready) {
        return Event_timedwait(e, ns);
    } else {
        return result;
    }
}

int
Event_signal(Event e, void *value)
{
    assert(e);
    int fail = pthread_mutex_lock(&e->mutex);
    if (!fail) {
        e->state = EventState_Ready;
        e->val = value;
        fail = pthread_cond_signal(&e->cond);
        if (!fail) {
            return pthread_mutex_unlock(&e->mutex);
        } else {
            return fail;
        }
    } else {
        return fail;
    }
}

int
Event_try_signal(Event e, void *value)
{
    assert(e);
    int fail = pthread_mutex_trylock(&e->mutex);
    if (!fail) {
        e->state = EventState_Ready;
        e->val = value;
        fail = pthread_cond_signal(&e->cond);
        if (!fail) {
            return pthread_mutex_unlock(&e->mutex);
        } else {
            return fail;
        }
    } else {
        return fail;
    }
}

int
Event_broadcast(Event e, void *value)
{
    assert(e);
    int fail = pthread_mutex_lock(&e->mutex);
    if (!fail) {
        e->state = EventState_Ready;
        e->val = value;
        fail = pthread_cond_broadcast(&e->cond);
        if (!fail) {
            return pthread_mutex_unlock(&e->mutex);
        } else {
            return fail;
        }
    } else {
        return fail;
    }
}

int
Event_try_broadcast(Event e, void *value)
{
    assert(e);
    int fail = pthread_mutex_trylock(&e->mutex);
    if (!fail) {
        e->state = EventState_Ready;
        e->val = value;
        fail = pthread_cond_broadcast(&e->cond);
        if (!fail) {
            return pthread_mutex_unlock(&e->mutex);
        } else {
            return fail;
        }
    } else {
        return fail;
    }
}

void
Event_set_value(Event e, void *value)
{
    assert(e);
    e->val = value;
}

void *
Event_value(Event e)
{
    assert(e);
    return e->val;
}

void
Event_free(Event *e)
{
    assert(e && *e);
    pthread_mutex_destroy( &(*e)->mutex );
    pthread_cond_destroy( &(*e)->cond );
    FREE(*e);
}
