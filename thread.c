#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem.h"
#include "thread.h"

struct LightningThread {
    pthread_t threadId;
};

LightningThread
LightningThread_create(LightningThreadFunction f, LightningThreadArgument arg)
{
    LightningThread t;
    NEW(t);
    int error = pthread_create(&t->threadId, NULL, f, arg);
    if (error) {
        fprintf(stderr, "Could not create thread (Error %d)\n", error);
        exit(EXIT_FAILURE);
    }
    return t;
}

int
LightningThread_join(LightningThread t)
{
    assert(t);
    return pthread_join(t->threadId, NULL);
}

void
LightningThread_free(LightningThread *t)
{
    assert(t && *t);
    FREE(*t);
}
