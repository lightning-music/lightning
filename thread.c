#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "mem.h"
#include "thread.h"

struct Thread {
    pthread_t threadId;
};

Thread
Thread_create(ThreadFunction f, ThreadArgument arg)
{
    Thread t;
    NEW(t);
    int error = pthread_create(&t->threadId, NULL, f, arg);
    if (error) {
        fprintf(stderr, "Could not create thread (Error %d)\n", error);
        exit(EXIT_FAILURE);
    }
    return t;
}

int
Thread_join(Thread t)
{
    assert(t);
    return pthread_join(t->threadId, NULL);
}

void
Thread_free(Thread *t)
{
    assert(t && *t);
    FREE(*t);
}
