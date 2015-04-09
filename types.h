#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdint.h>
#include <jack/jack.h>

#define SAMPLE_SIZE sizeof(sample_t)

typedef enum {
    SampleType_RAM,
    SampleType_DISK
} SampleType;

/**
 * Compare two opaque types
 * Return negative if a < b
 * Return 0        if a == b
 * Return positive if a > b
 */
typedef int (* CmpFunction)(const void *a, const void *b);

/**
 * Tempo in BPM
 */
typedef int tempo_t;

/**
 * Timeline position for metro
 */
typedef uint64_t position_t;

/**
 * Gain type float in [0, 1]
 */
typedef double gain_t;

/**
 * Pitch type
 */
typedef double pitch_t;

/**
 * default sample size (jack currently uses 32-bit float)
 */
typedef jack_default_audio_sample_t sample_t;

/**
 * number of channels
 */
typedef int channels_t;

/**
 * sample count
 */
typedef double sample_count_t;

/**
 * alias of jack_nframes_t (type used to represent sample frame counts)
 */
typedef jack_nframes_t nframes_t;

/**
 * callback for getting @a frames of sample data
 * return 0 for success, nonzero for failure
 */
typedef int (* AudioCallback)(sample_t **bufs,
                              channels_t channels,
                              nframes_t frames,
                              void *client_data);

typedef int (* SampleRateCallback)(nframes_t sr,
                                   void *arg);

typedef struct AudioData {
    /* frames available from the input buffer */
    nframes_t input_frames;
    /* frames we should write to output buffer */
    nframes_t output_frames;
    /* input buffer */
    sample_t *input;
    /* output buffer */
    sample_t *output;
} AudioData;

#endif
