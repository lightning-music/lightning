#ifndef SAMPLE_RAM_H_INCLUDED
#define SAMPLE_RAM_H_INCLUDED

#include "lightning.h"

typedef struct SampleRam *SampleRam;

/**
 * Play a sample.
 * This will either load cached sample data (fast) or
 * read sample data from disk (slow).
 * Reading sample data from disk should only happen the first
 * time you load a particular sample. After that it should
 * be cached and subsequent calls to Sample_play should
 * be much faster.
 */
SampleRam
SampleRam_init(const char *file,
               pitch_t pitch,
               gain_t gain,
               nframes_t output_samplerate);

SampleRam
SampleRam_clone(SampleRam orig,
                pitch_t pitch,
                gain_t gain,
                nframes_t output_samplerate);

/**
 * Get the path this sample was loaded from.
 */
const char *
SampleRam_path(SampleRam samp);

/**
 * Write sample data to some buffers.
 * Returns the number of frames written.
 */
nframes_t
SampleRam_write(SampleRam samp,
                sample_t **buffers,
                channels_t channels,
                nframes_t frames);

int
SampleRam_done(SampleRam samp);

/**
 * Make current thread wait for the sample to finish playing.
 */
int
SampleRam_wait(SampleRam samp);

/**
 * Free resources associated with a Sample.
 */
void
SampleRam_free(SampleRam *samp);

#endif
