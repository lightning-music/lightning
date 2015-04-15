#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "mem.h"

void *
lightning_mem_alloc(long nbytes,
          const char * file,
          int line) {
    void *p;
    assert(nbytes > 0);
    p = malloc(nbytes);
    // crash if oom
    assert(p != NULL);
    return p;
}

void *
lightning_mem_calloc(int count,
           long nbytes,
           const char * file,
           int line) {
    void *p;
    assert(count > 0);
    assert(nbytes > 0);
    p = calloc(count, nbytes);
    assert(p != NULL);
    return p;
}

void
lightning_mem_free(void * ptr,
         const char * file,
         int line) {
    if (ptr)
        free(ptr);
}

void *
lightning_mem_resize(void * ptr,
           long nbytes,
           const char * file,
           int line) {
    assert(ptr);
    assert(nbytes > 0);
    ptr = realloc(ptr, nbytes);
    assert(ptr != NULL);
    return ptr;
}

