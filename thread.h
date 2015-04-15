#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

typedef void * (* LightningThreadFunction)(void *);

typedef void * LightningThreadArgument;

typedef struct LightningThread *LightningThread;

LightningThread
LightningThread_create(LightningThreadFunction f, LightningThreadArgument arg);

int
LightningThread_join(LightningThread t);

void
LightningThread_free(LightningThread *t);

#endif
