#ifndef LIGHTNING_H_INCLUDED
#define LIGHTNING_H_INCLUDED

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

/**
 * Main lightning data structure.
 *
 * All functions in this interface will fail if
 * passed NULL instead of a properly initialized
 * Lightning instance.
 *
 * Typically an application will use a Lightning
 * instance for its entire life, so you may never need
 * to call Lightning_free()
 */
typedef struct Lightning *Lightning;

/**
 * Entrypoint for liblightning.
 *
 * The server also broadcasts messages over OSC/websocket
 * that provide clients a way to know what it is doing
 * internally.
 */
Lightning
Lightning_init();

/**
 * Connect lightning to a pair of JACK sinks.
 * @param lightning Lightning instance
 * @param ch1 first JACK output channel
 * @param ch2 second JACK output channel
 * @return 0 (success), nonzero (failure)
 */
int
Lightning_connect_to(Lightning lightning, const char *ch1, const char *ch2);

/**
 * Play a sample.
 * @param lightning Lightning instance
 * @param file Audio file to play
 * @param pitch Playback speed. `1.0` is normal speed, `0.5` half speed, and so on.
 * @param gain Gain [0.0, 1.0]
 * @return 0 success, nonzero failure
 */
int
Lightning_play_sample(Lightning lightning, const char *file,
                      pitch_t pitch, gain_t gain);

/**
 * Start exporting to an audio file
 * @param lightning Lightning instance
 * @return 0 success, nonzero failure
 */
int
Lightning_export_start(Lightning lightning, const char *file);

/**
 * If currently exporting, stop.
 * @param lightning Lightning instance
 * @return 0 success, nonzero failure
 */
int
Lightning_export_stop(Lightning lightning);

/**
 * Lightning_wait causes the current thread to wait for all
 * currently playing samples to finish.
 * @param lightning Lightning instance
 * @return 0 on success, nonzero otherwise.
 */
int
Lightning_wait(Lightning lightning);

/**
 * Free the system resources associated with a Lightning instance.
 *
 * @param lightning pointer to a Lightning instance
 */
void
Lightning_free(Lightning *lightning);

#endif
