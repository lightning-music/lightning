#include <assert.h>

#include "mem.h"
#include "mutex.h"
#include "realtime.h"

typedef enum {
    Initializing,
    Processing,
    Finished
} State;

struct Realtime {
    State state;
    Mutex mutex;
};

Realtime
Realtime_init() {
    Realtime rt;
    NEW(rt);
    rt->mutex = Mutex_init();
    rt->state = Initializing;
    return rt;
}

int
Realtime_set_initializing(Realtime rt) {
    assert(rt);
    int locked = Mutex_lock(rt->mutex);
    if (locked) {
        return locked;
    } else {
        rt->state = Initializing;
        return Mutex_unlock(rt->mutex);
    }
}

int
Realtime_set_processing(Realtime rt) {
    assert(rt);
    int locked = Mutex_lock(rt->mutex);
    if (locked) {
        return locked;
    } else {
        rt->state = Processing;
        return Mutex_unlock(rt->mutex);
    }
    return 0;
}

int
Realtime_set_finished(Realtime rt) {
    assert(rt);
    int locked = Mutex_lock(rt->mutex);
    if (locked) {
        return locked;
    } else {
        rt->state = Finished;
        return Mutex_unlock(rt->mutex);
    }
    return 0;
}

int
Realtime_is_initializing(Realtime rt) {
    assert(rt);
    return rt->state == Initializing;
}

int
Realtime_is_processing(Realtime rt) {
    assert(rt);
    return rt->state == Processing;
}

int
Realtime_is_finished(Realtime rt) {
    assert(rt);
    return rt->state == Finished;
}

void
Realtime_free(Realtime *rt) {
    assert(rt && *rt);
    Mutex_free(&(*rt)->mutex);
    FREE(*rt);
}
