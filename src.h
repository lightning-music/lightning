/**
 * Sample Rate Conversion
 * This is a wrapper around libsamplerate
 */
#ifndef SRC_H_INCLUDED
#define SRC_H_INCLUDED

#include "lightning.h"

typedef struct SRC *SRC;

/**
 * Initialize a Sample Rate Converter
 */
SRC
SRC_init();

/**
 * Process some audio data.
 *
 * src_ratio - Output sample rate divided by input sample rate
 * data - Audio data
 * input_frames_used - Set to the number of frames consumed from the input buffer
 * output_frames_gen - Set to the number of frames written to the output buffer
 */
int
SRC_process(SRC src,
            /* sample rate conversion ratio */
            double src_ratio,
            /* audio buffer data */
            AudioData data,
            /* frames used from input buffer */
            nframes_t *input_frames_used,
            /* frames generated in output buffer */
            nframes_t *output_frames_gen,
            /* will be non-zero if we hit the end of the input buffer */
            int *end);

const char *
SRC_strerror(int error);

/**
 * Free system resources allocated by a Sample Rate Converter
 */
void
SRC_free(SRC *src);

#endif // SRC_H_INCLUDED
