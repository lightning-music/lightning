/**
 * jack client
 * see http://jackaudio.org
 */
#include <assert.h>
#include <jack/jack.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "event.h"
#include "export-thread.h"
#include "jack-client.h"
#include "lightning.h"
#include "log.h"
#include "mem.h"
#include "mutex.h"
#include "ringbuffer.h"
#include "thread.h"

typedef enum {
    JackClientState_Initializing,
    JackClientState_Processing,
    JackClientState_Finished
} JackClientState;

struct JackClient {
    const char **ports;
    const char *server_name;
    void *data;
    jack_client_t *jack_client;
    jack_port_t *jack_output_port_1;
    jack_port_t *jack_output_port_2;
    AudioCallback audio_callback;
    sample_t **buffers;
    /* client state */
    JackClientState state;
    Mutex state_mutex;
    /* Thread for exporting to audio file */
    ExportThread export_thread;
};

/* state-handling functions */

static int
JackClient_set_state(JackClient client, JackClientState state)
{
    assert(client->state_mutex);
    int result = Mutex_lock(client->state_mutex);
    if (result) {
        return result;
    } else {
        client->state = state;
        return Mutex_unlock(client->state_mutex);
    }
}

inline static int
JackClient_is_processing(JackClient client)
{
    return client->state == JackClientState_Processing;
}

/* called by JACK whenever the server's sample rate changes */
static int
samplerate_callback(nframes_t sr,
                    void *data)
{
    /* Notify client code that depends on the output sample rate */
    return 0;
}

/* JACK shutdown callback */
/* Set a JackClient's state to finished */
void
jack_shutdown(void *arg)
{
    JackClient client = (JackClient) arg;
    JackClient_set_state(client, JackClientState_Finished);
}

/**
 * JACK error callback
 */
void
jack_errors(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

/**
 * JACK process callback
 */
int
process(jack_nframes_t nframes, void *arg)
{
    JackClient client = (JackClient) arg;
    /* return if the client is not ready for processing */
    if (! JackClient_is_processing(client)) {
        return 0;
    }
    /* setup output sample buffers */
    client->buffers[0] = jack_port_get_buffer(client->jack_output_port_1, nframes);
    client->buffers[1] = jack_port_get_buffer(client->jack_output_port_2, nframes);
    /* write data to the output buffer */
    int result = client->audio_callback(client->buffers,
                                        2,
                                        (nframes_t) nframes,
                                        client->data);
    /* possible write data to an audio file */
    ExportThread_write(client->export_thread, client->buffers, nframes);
    return result;
}

JackClient
JackClient_init(AudioCallback audio_callback, void *client_data)
{
    JackClient client;
    NEW(client);
    /* initialize state mutex and set state to Initializing */
    client->state_mutex = Mutex_init();
    if (JackClient_set_state(client, JackClientState_Initializing)) {
        fprintf(stderr, "Could not set JackClient state to Initializing\n");
        exit(EXIT_FAILURE);
    }
    /* open jack client */
    client->jack_client = jack_client_open("lightning", JackNullOption, NULL);
    if (client->jack_client == 0) {
        fprintf(stderr, "Jack server not running?\n");
        exit(EXIT_FAILURE);
    }
    nframes_t sr = jack_get_sample_rate(client->jack_client);
    client->data = client_data;
    client->audio_callback = audio_callback;
    client->export_thread = ExportThread_create(sr, 2);
    client->buffers = CALLOC(2, sizeof(sample_t*));
    return client;
}

/**
 * Register callbacks with JACK.
 */
int
JackClient_setup_callbacks(JackClient client)
{
    /* register error callback */
    jack_set_error_function(jack_errors);
    /* register shutdown callback */
    jack_on_shutdown(client->jack_client, jack_shutdown, client);
    /* register realtime callback */
    if (jack_set_process_callback(client->jack_client, process, client)) {
        fprintf(stderr, "Could not register process callback\n");
        exit(EXIT_FAILURE);
    }
    /* register a callback for when jack changes the output sample rate */
    if (jack_set_sample_rate_callback(client->jack_client,
                                      samplerate_callback,
                                      client)) {
        fprintf(stderr, "Could not register samplerate callback\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int
JackClient_activate(JackClient client)
{
    assert(client);
    return jack_activate(client->jack_client);
}

int
JackClient_setup_ports(JackClient client)
{
    assert(client);
    /* register output ports */
    client->jack_output_port_1 = \
        jack_port_register(client->jack_client,
                           "output_1",
                           JACK_DEFAULT_AUDIO_TYPE,
                           JackPortIsOutput,
                           0);

    client->jack_output_port_2 = \
        jack_port_register(client->jack_client,
                           "output_2",
                           JACK_DEFAULT_AUDIO_TYPE,
                           JackPortIsOutput,
                           0);

    if (NULL == client->jack_output_port_2) {
        fprintf(stderr, "wtf\n");
        exit(EXIT_FAILURE);
    }
    /* set state to Processing */
    if (JackClient_set_state(client, JackClientState_Processing)) {
        fprintf(stderr, "Could not set JackClient state to Processing\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int
JackClient_connect_to(JackClient client, const char *ch1, const char *ch2)
{
    assert(client);

    int err = jack_connect(client->jack_client,
                           jack_port_name(client->jack_output_port_1),
                           ch1);
    /* connect playback_1 */
    if (err) {
        LOG(Error, "Could not connect %s to %s\n",
            jack_port_name(client->jack_output_port_1),
            ch1);
        return err;
    }
    /* connect playback_2 */
    err = jack_connect(client->jack_client,
                       jack_port_name(client->jack_output_port_2),
                       ch2);
    if (err) {
        LOG(Error, "Could not connect %s to %s\n",
            jack_port_name(client->jack_output_port_2),
            ch2);
        return err;
    }
    return 0;
}

void
JackClient_set_data(JackClient client, void *data)
{
    assert(client);
    client->data = data;
}

nframes_t
JackClient_samplerate(JackClient jack)
{
    assert(jack);
    return jack_get_sample_rate(jack->jack_client);
}

int
JackClient_set_samplerate_callback(JackClient jack,
                                   SampleRateCallback callback,
                                   void *arg)
{
    assert(jack);
    return jack_set_sample_rate_callback(jack->jack_client,
                                         callback,
                                         arg);
}

nframes_t
JackClient_buffersize(JackClient jack)
{
    assert(jack);
    return jack_get_buffer_size(jack->jack_client);
}

int
JackClient_playback_ports(JackClient jack)
{
    return 2;
}

/**
 * Start exporting to an audio file
 * Return 0 on success, nonzero on failure
 */
int
JackClient_export_start(JackClient client, const char *file)
{
    assert(client && client->export_thread);
    LOG(Debug, "starting export for %s", file);
    size_t len = strlen(file);
    char *copy = ALLOC( len + 1 );
    memcpy(copy, file, len);
    copy[len] = '\0';
    return ExportThread_start(client->export_thread, copy);
}

/**
 * Stop recording output to audio file
 * Return 0 on success, nonzero on failure
 */
int
JackClient_export_stop(JackClient client)
{
    assert(client && client->export_thread);
    return ExportThread_stop(client->export_thread);
}

void
JackClient_free(JackClient *jack)
{
    assert(jack && *jack);
    JackClient j = *jack;
    JackClient_set_state(*jack, JackClientState_Finished);
    Mutex_free(&j->state_mutex);
    /* I don't think we should have to care about freeing the
       resources created for this thread since this thread exists
       for the entire duration of our program */
    /* ExportThread_free(&j->export_thread); */
    /* close jack client */
    FREE(j->buffers);
    jack_client_close(j->jack_client);
    ExportThread_free(&j->export_thread);
    FREE(*jack);
}
