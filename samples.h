#ifndef SAMPLES_H_INCLUDED
#define SAMPLES_H_INCLUDED

#define MAX_POLYPHONY 64

#include <types.h>

#include "sample.h"

/**
 * Polyphonic sample playback
 */
typedef struct Samples *Samples;

Samples
Samples_init(nframes_t output_sr);

/**
 * Add a directory to the list of directories
 * searched for audio files.
 * Returns 0 if the directory exists and was
 * added to the list, and nonzero if the directory
 * didn't exist or couldn't be added to the list (OOM)
 */
int
Samples_add_dir(Samples samps, const char *dir);

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
 * @param  {Samples}        samps - Samples instance
 * @param  {const char *}   path - path to audio file
 */
Sample
Samples_play(Samples samps,
             const char *path,
             pitch_t pitch,
             gain_t gain);

int
Samples_write(Samples samps,
              sample_t **buffers,
              channels_t channels,
              nframes_t frames);

void
Samples_free(Samples *cache);

#endif
