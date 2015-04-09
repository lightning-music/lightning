#include "jack-client.h"
#include "sample.h"
#include "samples.h"
#include "types.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int
stereo_callback(sample_t **buffers,
                channels_t channels,
                nframes_t frames,
                void *data) {
    Samples samps = (Samples) data;
    Samples_write(samps, buffers, channels, frames);
    return 0;
}

void
usage_and_exit(char *prog) {
    printf("Usage\n");
    printf("%s path/to/audio/file [pitch] [gain]\n", prog);
    exit(EXIT_FAILURE);
}

int
main(int argc, char **argv) {
    char *f;
    pitch_t pitch = 1.0;
    gain_t gain = 1.0;

    /* require path to audio file */

    if (argc < 2) {
        usage_and_exit(argv[0]);
    }

    f = argv[1];

    /* see if pitch and gain were provided */

    switch(argc) {
    case 4:
        gain = atof(argv[3]);
    case 3:
        pitch = atof(argv[2]);
        break;
    }

    /* initialize sample and jack client */

    JackClient jack_client = JackClient_init(stereo_callback, NULL);

    Samples samps = Samples_init(JackClient_samplerate(jack_client));

    JackClient_set_data(jack_client, samps);

    /* register a callback for if the jack output sample
       rate changes */

    JackClient_setup_callbacks(jack_client);

    JackClient_activate(jack_client);
    JackClient_setup_ports(jack_client);

    JackClient_connect_to(jack_client, "system:playback_1", "system:playback_2");

    Sample s = Samples_play(samps, f, pitch, gain);

    /* wait for sample to finish playing */

    Sample_wait(s);

    /* free sample and jack client */

    JackClient_free(&jack_client);
    Samples_free(&samps);

    return 0;
}
