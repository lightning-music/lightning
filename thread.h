#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

typedef void * (* ThreadFunction)(void *);

typedef void * ThreadArgument;

typedef struct Thread *Thread;

Thread
Thread_create(ThreadFunction f, ThreadArgument arg);

int
Thread_join(Thread t);

void
Thread_free(Thread *t);

#endif
