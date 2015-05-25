#ifndef SAMPLES_H_INCLUDED
#define SAMPLES_H_INCLUDED

#define MAX_POLYPHONY 64

#include "lightning.h"
#include "sample.h"

/**
 * Polyphonic sample playback.
 */
typedef struct Samples *Samples;

/**
 * Initialize a Sample object.
 */
Samples
Samples_init(nframes_t output_sr);

/**
 * Load a sample into the cache.
 * Do nothing if the sample was already loaded.
 */
Sample
Samples_load(Samples samps,
             const char *path);

/**
 * Get a new instance of the sample specified by path.
 * If the sample was not in the cache and had to be loaded from disk,
 * it will be stored in the cache so that subsequent loads
 * will be from memory.
 *
 * @param {Samples} samps - Samples instance
 * @param {const char *} path - path to audio file
 */
Sample
Samples_play(Samples samps,
             const char *path,
             pitch_t pitch,
             gain_t gain);

/**
 * Samples_write writes the data for all currently playing samples
 * to a pair of stereo buffers.
 * @return 0 on success, nonzero on failure.
 */
int
Samples_write(Samples samps,
              sample_t **buffers,
              channels_t channels,
              nframes_t frames);

/**
 * Samples_wait causes the current thread to wait until
 * all currently playing samples have finished.
 * @return 0 on success, nonzero otherwise
 */
int
Samples_wait(Samples samps);

/**
 * Free the resources held by a Samples object.
 */
void
Samples_free(Samples *cache);

#endif
