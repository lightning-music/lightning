/**
 * Top-level module for lightning.
 */
#include <assert.h>
#include <string.h>

#include "jack-client.h"
#include "lightning.h"
#include "log.h"
#include "mem.h"
#include "samples.h"

struct Lightning {
    JackClient jack_client;
    Samples samples;
};

/* realtime callback */
static int
audio_callback(sample_t **buffers, channels_t channels,
               nframes_t frames, void *data);
               
/**
 * Initialize jack_client and samples,
 * and use samples as the data for the jack_client
 * realtime callback.
 */
static void
initialize_jack_client(Lightning lightning);

Lightning
Lightning_init()
{
    Lightning lightning;
    NEW(lightning);
    initialize_jack_client(lightning);
    return lightning;
}

int
Lightning_connect_to(Lightning lightning, const char *ch1, const char *ch2)
{
    assert(lightning);
    return JackClient_connect_to(lightning->jack_client, ch1, ch2);
}

int
Lightning_add_dir(Lightning lightning, const char *dir)
{
    assert(lightning);
    return Samples_add_dir(lightning->samples, dir);
}

/**
 * Play a sample
 */
int
Lightning_play_sample(Lightning lightning,
                      const char *file,
                      pitch_t pitch,
                      gain_t gain)
{
    assert(lightning && lightning->samples);
    return NULL == Samples_play(lightning->samples, file, pitch, gain);
}

/**
 * Start exporting to an audio file
 */
int
Lightning_export_start(Lightning lightning, const char *file)
{
    assert(lightning);
    return JackClient_export_start(lightning->jack_client, file);
}

/**
 * If currently exporting, stop
 */
int
Lightning_export_stop(Lightning lightning)
{
    assert(lightning);
    return JackClient_export_stop(lightning->jack_client);
}

void
Lightning_free(Lightning *lightning)
{
    assert(lightning && *lightning);
    Lightning s = *lightning;
    Samples_free(&s->samples);
    JackClient_free(&s->jack_client);
    FREE(*lightning);
}

static int
audio_callback(sample_t **buffers,
               channels_t channels,
               nframes_t frames,
               void *data)
{
    Samples samples = (Samples) data;
    Samples_write(samples, buffers, channels, frames);
    return 0;
}

static void
initialize_jack_client(Lightning lightning)
{
    lightning->jack_client =                    \
        JackClient_init(audio_callback, NULL);

    lightning->samples =                                                \
        Samples_init(JackClient_samplerate(lightning->jack_client));
    
    JackClient_set_data(lightning->jack_client, lightning->samples);
    JackClient_setup_callbacks(lightning->jack_client);
    JackClient_activate(lightning->jack_client);
    JackClient_setup_ports(lightning->jack_client);
}
