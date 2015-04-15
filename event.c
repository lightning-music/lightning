#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <time.h>

#include "event.h"
#include "mem.h"

typedef enum {
    LightningEventState_NotReady,
    LightningEventState_Ready
} LightningEventState;

struct LightningEvent {
    void *val;
    LightningEventState state;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

LightningEvent
LightningEvent_init()
{
    LightningEvent e;
    NEW(e);
    pthread_mutex_init(&e->mutex, NULL);
    pthread_cond_init(&e->cond, NULL);
    e->state = LightningEventState_NotReady;
    e->val = NULL;
    return e;
}

int
LightningEvent_lock(LightningEvent e)
{
    assert(e);
    return pthread_mutex_lock(&e->mutex);
}

int
LightningEvent_wait(LightningEvent e)
{
    assert(e);
    e->state = LightningEventState_NotReady;
    int result = pthread_cond_wait(&e->cond, &e->mutex);
    if (e->state != LightningEventState_Ready) {
        return LightningEvent_wait(e);
    } else {
        return result;
    }
}

int
LightningEvent_timedwait(LightningEvent e, long ns)
{
    assert(e);
    e->state = LightningEventState_NotReady;
    struct timespec time;
    time.tv_nsec = ns;
    int result = pthread_cond_timedwait(&e->cond, &e->mutex, &time);
    if (e->state != LightningEventState_Ready) {
        return LightningEvent_timedwait(e, ns);
    } else {
        return result;
    }
}

int
LightningEvent_signal(LightningEvent e, void *value)
{
    assert(e);
    int fail = pthread_mutex_lock(&e->mutex);
    if (!fail) {
        e->state = LightningEventState_Ready;
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
LightningEvent_try_signal(LightningEvent e, void *value)
{
    assert(e);
    int fail = pthread_mutex_trylock(&e->mutex);
    if (!fail) {
        e->state = LightningEventState_Ready;
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
LightningEvent_broadcast(LightningEvent e, void *value)
{
    assert(e);
    int fail = pthread_mutex_lock(&e->mutex);
    if (!fail) {
        e->state = LightningEventState_Ready;
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
LightningEvent_try_broadcast(LightningEvent e, void *value)
{
    assert(e);
    int fail = pthread_mutex_trylock(&e->mutex);
    if (!fail) {
        e->state = LightningEventState_Ready;
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
LightningEvent_set_value(LightningEvent e, void *value)
{
    assert(e);
    e->val = value;
}

void *
LightningEvent_value(LightningEvent e)
{
    assert(e);
    return e->val;
}

void
LightningEvent_free(LightningEvent *e)
{
    assert(e && *e);
    pthread_mutex_destroy( &(*e)->mutex );
    pthread_cond_destroy( &(*e)->cond );
    FREE(*e);
}
