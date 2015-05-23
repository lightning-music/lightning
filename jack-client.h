/**
 * jack client
 * see http://jackaudio.org
 *
 * Goals:
 * - Expose an interface
 */
#ifndef JACK_CLIENT_H_INCLUDED
#define JACK_CLIENT_H_INCLUDED

#include <stddef.h>

#include "lightning.h"

/**
 * AudioEngine data structure.
 */
typedef struct JackClient *JackClient;

/**
 * Initialize an audio engine.
 * @param realtime callback used to fill frame buffer
 * @client_data pointer to data passed to callback
 */
JackClient
JackClient_init(AudioCallback audio_callback, void *client_data);

/**
 * Register callbacks for a JackClient.
 *
 * @param client   {JackClient}
 */
int
JackClient_setup_callbacks(JackClient client);

/**
 * Activate a JackClient.
 *
 * @param client   {JackClient}
 */
int
JackClient_activate(JackClient client);

/**
 * Setup the output ports for a JackClient.
 *
 * @param client   {JackClient}
 *
 * @return 0 (success), nonzero (failure)
 */
int
JackClient_setup_ports(JackClient client);

/**
 * Connect JackClient's output to a pair of jack inputs.
 *
 * e.g. to connect to the system outputs
 * JackClient_connect(client, "system:playback_1", "system:playback_2");
 *
 * @param client   {JackClient}
 * @param ch1      {const char *}
 * @param ch2      {const char *}
 *
 * @return 0 (success), nonzero (failure)
 */
int
JackClient_connect_to(JackClient client, const char *ch1, const char *ch2);

void
JackClient_set_data(JackClient client, void *data);

nframes_t
JackClient_samplerate(JackClient jack);

int
JackClient_set_samplerate_callback(JackClient jack,
                                   SampleRateCallback callback,
                                   void *arg);

nframes_t
JackClient_buffersize(JackClient jack);

int
JackClient_playback_ports(JackClient jack);

/**
 * Start exporting to an audio file
 *
 * @return 0 on success, nonzero on failure
 */
int
JackClient_export_start(JackClient client, const char *file);

/**
 * Stop recording output to audio file
 *
 * @return 0 on success, nonzero on failure
 */
int
JackClient_export_stop(JackClient client);

// free any resources being used by the audio engine
void
JackClient_free(JackClient *jack);

#endif
