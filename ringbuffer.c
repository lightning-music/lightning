#include <assert.h>
#include <jack/ringbuffer.h>
#include <types.h>
#include <stddef.h>

#include "mem.h"
#include "ringbuffer.h"

struct Ringbuffer {
    jack_ringbuffer_t *jrb;
};

Ringbuffer
Ringbuffer_init(size_t size)
{
    Ringbuffer rb;
    NEW(rb);
    rb->jrb = jack_ringbuffer_create(size);
    return rb;
}

int
Ringbuffer_mlock(Ringbuffer rb)
{
    assert(rb);
    return jack_ringbuffer_mlock(rb->jrb);
}

size_t
Ringbuffer_read(Ringbuffer rb,
                char *buf,
                size_t len)
{
    assert(rb);
    return jack_ringbuffer_read(rb->jrb, (void *) buf, len);
}

size_t
Ringbuffer_write(Ringbuffer rb, void *buf, size_t len)
{
    assert(rb);
    return jack_ringbuffer_write(rb->jrb, buf, len);
}

void
Ringbuffer_free(Ringbuffer *rb)
{
    assert(rb && *rb);
    jack_ringbuffer_free((*rb)->jrb);
    FREE(*rb);
}
