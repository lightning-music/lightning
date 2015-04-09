#include <assert.h>
#include <types.h>
#include <math.h>
/* #include <sndfile.h> */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clip.h"
#include "event.h"
#include "log.h"
#include "mem.h"
#include "mutex.h"
#include "sample-ram.h"
#include "sf.h"
#include "src.h"

typedef enum {
    Initializing,
    /* ready for processing */
    Processing,
    /* cleaning up after playing */
    Finished,
} State;

struct SampleRam {
    char *path;
    pitch_t pitch;
    gain_t gain;
    // channels, frames, and samplerate
    channels_t channels;
    nframes_t frames;
    int samplerate;
    // buffers to hold sample data (one per channel)
    sample_t **framebufs;
    // track read position in file (guarded by mutex)
    nframes_t framep;
    Mutex framep_mutex;
    nframes_t total_frames_written;
    // provide a way to synchronize a thread on the `done` event
    Event done_event;
    /* sample rate converters */
    double src_ratio;
    /* sample state and associated mutex */
    State state;
    Mutex state_mutex;
};

/* static utility functions */

static void
initialize_state(SampleRam s);

static void
allocate_frame_buffers(SampleRam s, nframes_t frames);

/**
 * Copy framebufs from src to dest. Time-scale with @a pitch, and
 * scale amplitude with gain.
 */
static void
copy_frame_buffers(SampleRam dest, SampleRam src, gain_t gain);

static int
SampleRam_set_state(SampleRam samp, State state);

static void
SampleRam_set_state_or_exit(SampleRam samp, State state);

static const char *
state_string(State state) {
    switch(state) {
    case Initializing:
        return "Initializing";
        break;
    case Processing:
        return "Processing";
        break;
    case Finished:
        return "Finished";
        break;
    default:
        assert(0);
    }
}

static inline int
SampleRam_is_processing(SampleRam samp)
{
    return samp->state == Processing;
}

/**
 * Set a sample's path.
 */
static void
SampleRam_set_path(SampleRam samp, const char *path)
{
    /* copy string to path member */
    size_t path_bytes = strlen(path);
    samp->path = ALLOC(path_bytes + 1);
    memcpy(samp->path, path, path_bytes);
    samp->path[path_bytes] = '\0';
}

/**
 * De-interleave and resample a buffer that has
 * been read from disk.
 */
static int
SampleRam_set_buffers(SampleRam samp, sample_t *buf, nframes_t output_sr)
{
    /* sample-rate converters */
    SRC srcs[2];
    srcs[0] = SRC_init();
    srcs[1] = SRC_init();
    /* de-interleaved buffers */
    sample_t *di_bufs[2];
    di_bufs[0] = ALLOC( samp->frames * SAMPLE_SIZE );
    di_bufs[1] = ALLOC( samp->frames * SAMPLE_SIZE );
    /* de-interleave */
    int i;
    unsigned int j;
    LOG(Info, "samp->channels = %d", samp->channels);
    if (samp->channels == 1) {
        for (j = 0; j < samp->frames; j++) {
            di_bufs[0][j] = di_bufs[1][j] = buf[j];
        }
    } else if (samp->channels == 2) {
        for (j = 0; j < samp->frames; j++) {
            for (i = 0; i < samp->channels; i++) {
                di_bufs[i][j] = buf[ (j * samp->channels) + i ];
            }
        }
    } else {
        LOG(Error, "Unsupported number of channels (%d). "
            "Only stereo and mono are supported.\n", samp->channels);
        return 1;
    }
    /* perform sample rate conversion */
    double src_ratio = output_sr / (double) samp->samplerate;
    nframes_t output_frames = (nframes_t) ceil(samp->frames * src_ratio);
    int error, end_of_input = 0;
    AudioData audio_data;
    nframes_t frames_consumed = 0, frames_produced = 0;
    nframes_t input_frames_used, output_frames_gen;
    while (frames_consumed < samp->frames) {
        input_frames_used = 0;
        output_frames_gen = 0;
        audio_data.input_frames = samp->frames - frames_consumed;
        audio_data.output_frames = output_frames - frames_produced;
        for (i = 0; i < 2; i++) {
            audio_data.output = &samp->framebufs[i][frames_produced];
            audio_data.input = &di_bufs[i][frames_consumed];
            error = SRC_process(srcs[i], src_ratio, audio_data,
                                &input_frames_used, &output_frames_gen,
                                &end_of_input);
            if (error) {
                LOG(Error, "Error in sample rate conversion: %s",
                    SRC_strerror(error));
            }
        }
        if (end_of_input) {
            break;
        } else {
            frames_consumed += input_frames_used;
            frames_produced += output_frames_gen;
        }
    }

    FREE(di_bufs[0]);
    FREE(di_bufs[1]);
    SRC_free(&srcs[0]);
    SRC_free(&srcs[1]);
    /* FREE(srcs[0]); */
    /* FREE(srcs[1]); */

    return 0;
}

/**
 * Read a sound file, de-interleave the channels if necessary,
 * perform sample rate conversion, then cache this data in memory.
 */
SampleRam
SampleRam_init(const char *file, pitch_t pitch, gain_t gain, nframes_t output_sr)
{
    SampleRam s;
    /* initialize state mutex and set state to Processing */
    NEW(s);
    initialize_state(s);
    SampleRam_set_path(s, file);
    /* open audio file */
    SF sf = SF_open_read(file);
    if (sf == NULL) {
        LOG(Warn, "could not open %s\n", file);
        FREE(s);
        return NULL;
    }
    /* Set pitch to a very small number if it is 0,
       otherwise clip it to a given range and
       if it is negative set the reversed bit */
    if (pitch == 0.0) {
        s->pitch = 0.0001;
    } else {
        s->pitch = clip(pitch, -32.0f, 32.0f);
    }
    s->gain = clip(gain, 0.0f, 1.0f);
    s->channels = SF_channels(sf);
    s->frames = SF_frames(sf);
    s->samplerate = SF_samplerate(sf);
    s->done_event = Event_init(NULL);
    /* allocate stereo buffers */
    double src_ratio = output_sr / (double) s->samplerate;
    nframes_t output_frames = (nframes_t) ceil(s->frames * src_ratio);
    allocate_frame_buffers(s, output_frames);
    /* read the file */
    /* some files seem to report a smaller number of frames than
       the data they actually contain.
       this code was segfault'ing when trying to read
       http://www.freesound.org/people/madjad/sounds/21653/ */
    const nframes_t frames = (4096 / SAMPLE_SIZE) / s->channels;
    sample_t *framebuf = ALLOC( (s->frames + frames) * s->channels * SAMPLE_SIZE );
    long total_frames = SF_read(sf, framebuf, frames);

    while (total_frames < s->frames) {
        total_frames +=                         \
            SF_read(sf, framebuf + (total_frames * s->channels), frames);
    }
    assert(total_frames == s->frames);
    SF_close(&sf);

    /* de-interleave (if necessary) and resample */
    SampleRam_set_buffers(s, framebuf, output_sr);

    FREE(framebuf);

    /* set frames member to the number of frames that are
       actually in framebuf after resampling */
    s->frames = output_frames;
    s->framep = 0;
    s->framep_mutex = Mutex_init();
    s->total_frames_written = 0;
    SampleRam_set_state_or_exit(s, Processing);
    LOG(Debug, "SampleRam_init: done loading %s", file);
    return s;
}

SampleRam
SampleRam_clone(SampleRam orig, pitch_t pitch, gain_t gain, nframes_t output_sr)
{
    LOG(Debug, "cloning sample %p", orig);
    SampleRam s;
    NEW(s);
    initialize_state(s);
    s->pitch = pitch;
    s->gain = gain;
    s->frames = orig->frames;
    s->channels = orig->channels;
    s->samplerate = orig->samplerate;
    s->src_ratio = output_sr / (double) orig->samplerate;
    s->done_event = Event_init(NULL);
    SampleRam_set_path(s, orig->path);
    allocate_frame_buffers(s, s->frames);
    copy_frame_buffers(s, orig, s->gain);
    s->framep = 0;
    s->framep_mutex = Mutex_init();
    s->total_frames_written = 0;
    SampleRam_set_state_or_exit(s, Processing);
    LOG(Debug, "%s created %p", "SampleRam_clone", s);
    return s;
}

/**
 * Path to loaded file.
 */
const char *
SampleRam_path(SampleRam samp)
{
    assert(samp);
    return samp->path;
}

nframes_t
SampleRam_write(SampleRam samp, sample_t **buffers, channels_t channels,
                nframes_t frames)
{
    assert(samp);

    if (! SampleRam_is_processing(samp)) {
        /* fprintf(stderr, "sample is not processing\n"); */
        return 0;
    }

    nframes_t len = samp->frames;
    /* we ensure at initialization that mono samples fill
       stereo buffers */
    int chans = 2;
    /* int chans = (int) samp->channels; */
    nframes_t offset = samp->framep;
    int chan = 0;
    long frame = 0;
    long prev_frame = 0;
    long input_frame = 0;
    double frame_index = 0.0;
    int at_end = 0;
    nframes_t frames_used = 0;

    while (frame < frames) {
        for (chan = 0; chan < chans; chan++) {
            if (offset + input_frame < len) {
                buffers[chan][frame] =                      \
                    samp->framebufs[chan][offset + input_frame];
            } else {
                at_end = 1;
                buffers[chan][frame] = 0.0f;
            }
        }
        if (!at_end) {
            /* store previous frame index */
            prev_frame = (long) frame_index;
            /* nudge the sample index forward
               this may not actually advance the frame pointer */
            frame_index = frame_index + samp->pitch;
            input_frame = (long) frame_index;
            frames_used += input_frame - prev_frame;
        }
        /* advance the output frame pointer */
        frame++;
    }

    if (at_end) {
        SampleRam_set_state(samp, Finished);
        Event_try_broadcast(samp->done_event, NULL);
    } else {
        samp->framep += frames_used;
    }

    return 0;
}

int
SampleRam_done(SampleRam samp)
{
    /* need to lock mutex? */
    return samp->state == Finished;
}

int
SampleRam_wait(SampleRam samp)
{
    assert(samp);
    return Event_wait(samp->done_event);
}

/**
 * Free resources associated with this sample.
 */
void
SampleRam_free(SampleRam *samp)
{
    int i;
    assert(samp && *samp);
    SampleRam s = *samp;
    /* free the path char array */
    FREE(s->path);
    /* free the done event */
    Event_free(&s->done_event);
    /* free the framep mutex */
    Mutex_free(&s->framep_mutex);
    /* free the state mutex */
    Mutex_free(&s->state_mutex);
    LOG(Debug, "SampleRam_free s->framebufs[0]  %p", s->framebufs[0]);
    LOG(Debug, "SampleRam_free s->framebufs[1]  %p", s->framebufs[1]);
    for (i = 0; i < s->channels; i++) {
        FREE(s->framebufs[i]);
    }
    FREE(s->framebufs);
    void *p = *samp;
    FREE(*samp);
    LOG(Debug, "freed %p", p);
}

static int
SampleRam_set_state(SampleRam samp, State state)
{
    assert(samp->state_mutex);
    int not_locked = Mutex_trylock(samp->state_mutex);
    if (not_locked) {
        return not_locked;
    } else {
        samp->state = state;
        return Mutex_unlock(samp->state_mutex);
    }
}

static void
SampleRam_set_state_or_exit(SampleRam s, State state)
{
    if (SampleRam_set_state(s, state)) {
        fprintf(stderr, "Could not set SampleRam state to %s\n",
                state_string(state));
        exit(EXIT_FAILURE);
    }
}

static void
initialize_state(SampleRam s)
{
    s->state_mutex = Mutex_init();
    SampleRam_set_state_or_exit(s, Initializing);
}

static void
allocate_frame_buffers(SampleRam s, nframes_t frames)
{
    size_t sz = frames * SAMPLE_SIZE;
    s->framebufs = CALLOC(2, sizeof(sample_t*));
    LOG(Debug, "allocating frame buffers of size %ld", sz);
    s->framebufs[0] = ALLOC(frames * SAMPLE_SIZE);
    s->framebufs[1] = ALLOC(frames * SAMPLE_SIZE);
    LOG(Debug, "allocated s->framebufs[0]  %p", s->framebufs[0]);
    LOG(Debug, "allocated s->framebufs[1]  %p", s->framebufs[1]);
}

/**
 * This function is used for loading cached samples.
 * It doesn't matter if we use src or dest in the loop
 * conditions.
 */
static void
copy_frame_buffers(SampleRam dest, SampleRam src, gain_t gain)
{
    int i;
    unsigned int j;
    for (j = 0; j < dest->frames; j++) {
        /* we deliberately initialize the samples in a way that
           fills stereo buffers even if the sample is mono on disk */
        /* for (i = 0; i < dest->channels; i++) { */
        for (i = 0; i < 2; i++) {
            dest->framebufs[i][j] = gain * src->framebufs[i][j];
        }
    }
}
