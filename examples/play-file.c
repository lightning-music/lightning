/**
 * \file play-file.c
 * A working example of how to play samples with jack-client
 * and src sample rate converter.
 */
#include "jack-client.h"
#include "src.h"
#include "types.h"

#include <assert.h>
#include <pthread.h>
#include <samplerate.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
    FP_INITIALIZING,
    FP_PROCESSING,
    FP_FINISHED
} FP_STATE;

typedef struct FilePlayer {
    // state variable
    FP_STATE state;
    // sndfile pointer
    SNDFILE *sf;
    // number of channels
    int channels;
    // number of frames
    int frames;
    // sample rate
    nframes_t samplerate;
    // length in samples = channels * frames
    long length;
    // frames we've read
    long frames_read;
    // sample data buffers
    sample_t **framebufs;
    // mutex and conditional variable
    // used to signal sample done
    pthread_cond_t done;
    pthread_mutex_t done_lock;
    // sample rate converters (1 per channel)
    SRC *src;
    double src_ratio;
} FilePlayer;

/**
 * Initialize a FilePlayer object
 */
static void
initialize_file_player(FilePlayer *fp,
                       const char *f,
                       nframes_t output_sample_rate) {
    assert(fp);
    int i;
    fp->state = FP_INITIALIZING;
    // initialize condition variable
    pthread_cond_init(&fp->done, NULL);
    pthread_mutex_init(&fp->done_lock, NULL);
    // open file
    SF_INFO sfinfo;
    SNDFILE *sf = sf_open(f, SFM_READ, &sfinfo);
    // initialize data members
    fp->channels = sfinfo.channels;
    fp->frames = sfinfo.frames;
    fp->samplerate = sfinfo.samplerate;
    fp->src_ratio = output_sample_rate / (double) fp->samplerate;
    // samples = frames * channels
    fp->length = fp->frames * fp->channels;
    // frames we have already read from the frame buffer
    fp->frames_read = 0;
    // allocate stereo frame buffers
    fp->framebufs = calloc(2, sizeof(sample_t*));
    fp->framebufs[0] = calloc(fp->frames, SAMPLE_SIZE);
    fp->framebufs[1] = calloc(fp->frames, SAMPLE_SIZE);

    assert(fp->framebufs);
    /* allocate the buffer for sf_read */
    sample_t framebuf[ fp->channels * fp->frames ];
    /* fill frame buffer */
    int frames_to_read = 4096;
    long total_frames_read = sf_readf_float(sf, framebuf, frames_to_read);
    while (total_frames_read < fp->frames) {
        /* adjust buffer pointer */
        total_frames_read +=                    \
            sf_readf_float(sf,
                           framebuf + (total_frames_read * fp->channels),
                           frames_to_read);
    }

    if (total_frames_read != fp->frames) {
        fprintf(stderr, "Could not read expected number of frames from %s\n", f);
        exit(EXIT_FAILURE);
    }

    /* de-interleave if stereo, fill stereo buffers if mono */
    int j;
    if (fp->channels == 1) {
        for (j = 0; j < fp->frames; j++) {
            fp->framebufs[0][j] = fp->framebufs[1][j] = framebuf[j];
        }
    } else if (fp->channels == 2) {
        for (j = 0; j < fp->frames; j++) {
            for (i = 0; i < fp->channels; i++) {
                fp->framebufs[i][j] = framebuf[ (j * fp->channels) + i ];
            }
        }
    } else {
        fprintf(stderr, "Audio file has %d channels. "
                "Only mono and stereo are currently supported.\n", fp->channels);
        exit(EXIT_FAILURE);
    }

    /* lock the done mutex */
    pthread_mutex_lock(&fp->done_lock);
    /* close file */
    sf_close(sf);
    /* initialize sample rate converter */
    fp->src = calloc(2, sizeof(SRC));
    fp->src[0] = SRC_init();
    fp->src[1] = SRC_init();
    /* set state to PROCESSING */
    fp->state = FP_PROCESSING;
}

/**
 * Destroy a FilePlayer object
 */
static void
free_file_player(FilePlayer *fp) {
    assert(fp);
    // free condition variable
    pthread_cond_destroy(&fp->done);
    /* free SRC's */
    SRC_free(&fp->src[0]);
    SRC_free(&fp->src[1]);
    free(fp->src);
    // free frame buffer
    int i;
    for (i = 0; i < fp->channels; i++) {
        free(fp->framebufs[i]); fp->framebufs[i] = NULL;
    }
}

/**
 * jack client realtime callback
 */
static int
audio_callback(sample_t **buffers,
               channels_t channels,
               nframes_t frames,
               void *data) {
    FilePlayer *fp = (FilePlayer *) data;

    if (fp->state != FP_PROCESSING) {
        return 0;
    }

    nframes_t frames_read = fp->frames_read;
    long offset = frames_read;
    AudioData audio_data;
    int end_of_input = 0;
    int chan = 0;
    int error = 0;
    nframes_t input_frames_used = 0;
    nframes_t output_frames_gen = 0;

    audio_data.input_frames = fp->frames - frames_read;
    audio_data.output_frames = frames;

    for (chan = 0; chan < channels; chan++) {
        audio_data.output = buffers[chan];
        audio_data.input = &fp->framebufs[chan][offset];

        input_frames_used = 0;
        output_frames_gen = 0;

        error = SRC_process(fp->src[chan], fp->src_ratio, audio_data,
                            &input_frames_used, &output_frames_gen,
                            &end_of_input);

        if (error) {
            fprintf(stderr, "Error in SRC_process: %s\n",
                    SRC_strerror(error));
            exit(EXIT_FAILURE);
        }
    }

    if (end_of_input) {
        fp->state = FP_FINISHED;
        pthread_cond_broadcast(&fp->done);
    } else {
        fp->frames_read += input_frames_used;
    }

    return 0;
}

void
usage_and_exit(char *prog) {
    fprintf(stderr, "Usage\n");
    fprintf(stderr, "$ %s path/to/audio/file\n", prog);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage_and_exit(argv[0]);
    }

    FilePlayer fp;

    // initialize jack client

    JackClient jack_client = \
        JackClient_init(audio_callback, &fp);

    // initialize FilePlayer

    initialize_file_player(&fp, argv[1],
                           JackClient_samplerate(jack_client));

    JackClient_setup_callbacks(jack_client);
    JackClient_activate(jack_client);
    JackClient_setup_ports(jack_client);

    // wait to be done

    pthread_cond_wait(&fp.done, &fp.done_lock);

    // free FilePlayer

    free_file_player(&fp);

    // free jack client

    JackClient_free(&jack_client);

    return 0;
}
