#ifndef SAMPLE_H_INCLUDED
#define SAMPLE_H_INCLUDED

#include "lightning.h"
#include "sample-disk.h"
#include "sample-ram.h"

typedef union Sample {
    SampleRam ram;
    SampleDisk disk;
} *Sample;

/**
 * Play a sample.
 * This will either load cached sample data (fast) or
 * read sample data from disk (slow).
 * Reading sample data from disk should only happen the first
 * time you load a particular sample. After that it should
 * be cached and subsequent calls to Sample_play should
 * be much faster.
 */
Sample
Sample_init(const char *file, pitch_t pitch,
            gain_t gain, nframes_t output_samplerate);

/**
 * Sample_clone clones a sample structure.
 * This is used to create clones of cached sample data.
 * The clone is then added to the play buffer for the rt callback to pick up.
 */
Sample
Sample_clone(Sample orig,
             pitch_t pitch,
             gain_t gain,
             nframes_t output_samplerate);

/**
 * Determine if the underlying sample structure is null.
 * This should be used to determine if there were any errors
 * initializing the sample.
 * Returns 1 if the sample could not be initialized, 0 otherwise.
 * If the sample could not be initialized, client code should free
 * the sample and figure out what went wrong by inspecting the logs.
 */
int
Sample_isnull(Sample samp);

/**
 * Get the path this sample was loaded from.
 */
const char *
Sample_path(Sample samp);

/**
 * Write sample data to some buffers.
 * Returns the number of frames written.
 */
nframes_t
Sample_write(Sample samp,
             sample_t **buffers,
             channels_t channels,
             nframes_t frames);

/**
 * Return 1 if the sample is done playing, 0 otherwise.
 */
int
Sample_done(Sample samp);

/**
 * Make current thread wait for the sample to finish playing.
 * Returns 0 if successful, nonzero otherwise.
 */
int
Sample_wait(Sample samp);

/**
 * Free resources associated with a Sample.
 */
void
Sample_free(Sample *samp);

#endif
