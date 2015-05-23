#ifndef RINGBUFFER_H_INCLUDED
#define RINGBUFFER_H_INCLUDED

#include <stddef.h>

#include "lightning.h"

typedef struct Ringbuffer *Ringbuffer;

/**
 * Initialize a Ringbuffer with @a size bytes
 */
Ringbuffer
Ringbuffer_init(size_t size);

/**
 * Lock a ringbuffer's memory into RAM.
 * Returns 0 on success, non-zero on failure.
 */
int
Ringbuffer_mlock(Ringbuffer rb);

/**
 * Read @a len samples from @a rb into @a buf.
 * Returns the number of samples read.
 */
size_t
Ringbuffer_read(Ringbuffer rb, char *buf, size_t len);

/**
 * Write @a len samples from @a buf to @a rb.
 * Returns the number of samples written.
 */
size_t
Ringbuffer_write(Ringbuffer rb, void *buf, size_t len);

/**
 * Free a ringbuffer.
 */
void
Ringbuffer_free(Ringbuffer *rb);

#endif
