
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "bin-tree.h"
#include "event.h"
#include "lightning.h"
#include "log.h"
#include "mem.h"
#include "realtime.h"
#include "ringbuffer.h"
#include "sample.h"
#include "samples.h"
#include "thread.h"

#define ASSUMED_CHANNELS 2

struct Samples {
    /* output sample rate */
    nframes_t output_sr;
    /* sample cache */
    BinTree cache;
    /* sample that are actively playing on any
       given audio cycle */
    Sample active[MAX_POLYPHONY];
    /* buffer where we pull samples that need to go
       in the active array */
    Ringbuffer play_buf;
    /* event used to signal that a Sample needs
       to be added to the play buffer */
    LightningEvent play_event;
    /* thread used to push samples into the play buffer
       every time there is a play event */
    LightningThread play_thread;
    /* buffer containing samples that need to be freed */
    Ringbuffer free_buf;
    /* event used to signal when there are samples that
       need to be freed */
    LightningEvent free_event;
    /* thread that frees samples */
    LightningThread free_thread;
    /* state for objects being used in the realtime thread */
    Realtime state;
    /* Sample pointer used for pushing samples into the play
       ringbuffer */
    Sample new_sample;
    /* summing buffers */
    sample_t **sum_bufs;
    /* sample collecting buffers */
    sample_t **collect_bufs;
    /* directories to search for audio files */
    char **dirs;
};

void *
play_new_samples(void *arg);

void *
free_done_samples(void *arg);

/**
 * Data for the thread we start that adds
 * Sample instances to a ringbuffer every
 * time we are requested to play one.
 * We spin up a dedicated thread just for this
 * task because jack ringbuffers are only considered
 * thread-safe if there is exactly one writer thread
 * and exactly one reader thread and that these threads
 * never change.
 */
typedef struct LightningThreadData {
    Ringbuffer buf;
    LightningEvent event;
} *LightningThreadData;

Samples
Samples_init(nframes_t output_sr)
{
    int i = 0;
    Samples samps;
    NEW(samps);
    samps->state = Realtime_init();
    samps->cache = BinTree_init((CmpFunction) strcmp);
    samps->dirs = NULL;

    /* allocate auxiliary buffers
       each buffer will be able to hold 2048 samples
       and will be mlock'ed into RAM
       there is one per channel and we just assume stereo */

    const size_t aux_buf_size = 2048;

    samps->sum_bufs = CALLOC(ASSUMED_CHANNELS, sizeof(sample_t*));
    samps->collect_bufs = CALLOC(ASSUMED_CHANNELS, sizeof(sample_t*));

    for (i = 0; i < ASSUMED_CHANNELS; i++) {
        samps->sum_bufs[i] = CALLOC(aux_buf_size, SAMPLE_SIZE);
        samps->collect_bufs[i] = CALLOC(aux_buf_size, SAMPLE_SIZE);

        if (0 != mlock(samps->sum_bufs[i], aux_buf_size * SAMPLE_SIZE)) {
            LOG(Error, "Could not lock memory into %s", "RAM");
        }

        if (0 != mlock(samps->collect_bufs[i], aux_buf_size * SAMPLE_SIZE)) {
            LOG(Error, "Could not lock memory into %s", "RAM");
        }
    }

    for (i = 0; i < MAX_POLYPHONY; i++) {
        samps->active[i] = NULL;
    }

    samps->output_sr = output_sr;

    /* setup play thread */

    LightningThreadData play_thread;
    NEW(play_thread);
    samps->play_buf = Ringbuffer_init(sizeof(Sample) * MAX_POLYPHONY);
    if (0 != Ringbuffer_mlock(samps->play_buf)) {
        LOG(Error, "Could not %s ringbuffer", "mlock");
    }
    samps->play_event = LightningEvent_init();
    play_thread->buf = samps->play_buf;
    play_thread->event = samps->play_event;
    samps->play_thread = LightningThread_create(play_new_samples, play_thread);

    /* setup free thread */

    LightningThreadData free_thread;
    NEW(free_thread);
    samps->free_event = LightningEvent_init();
    free_thread->event = samps->free_event;
    samps->free_thread = LightningThread_create(free_done_samples, free_thread);

    /* samps->new_sample = ALLOC(sizeof(Sample)); */

    if (Realtime_set_processing(samps->state)) {
        LOG(Error, "Could not set Samples state to processing%s", "");
    }
    return samps;
}

Sample
Samples_load(Samples samps, const char *path)
{
    assert(samps);
    LOG(Debug, "looking up %s in sample cache", path);
    Sample cached = (Sample) BinTree_lookup(samps->cache, path);
    if (NULL == cached) {
        LOG(Debug, "sample %s was not cached", path);
        /* initialize and cache it */
        Sample samp = Sample_init(path, 1.0, 1.0, samps->output_sr);
        /* Sample samp = Samples_find_file(samps, path, samps->output_sr); */
        if (samp != NULL) {
            LOG(Debug, "storing %s -> %p in cache", path, samp);
            BinTree_insert(samps->cache, path, samp);
        } else {
            LOG(Info, "could not open %s", path);
        }
        return samp;
    } else {
        LOG(Debug, "returning cached sample %p", cached);
        return cached;
    }
}

/**
 * Get a new instance of the sample specified by path.
 * If the sample was not in the cache and had to be loaded from disk,
 * it will be stored in the cache so that subsequent loads
 * will be from memory.
 */
Sample
Samples_play(Samples samps, const char *path, pitch_t pitch, gain_t gain)
{
    assert(samps);
    LOG(Debug, "playing %s", path);
    Sample cached = Samples_load(samps, path);
    if (Sample_isnull(cached)) {
        LOG(Error, "could not load %s", path);
        return NULL;
    }
    LOG(Debug, "loaded %p", cached);
    Sample samp = Sample_clone(cached, pitch, gain, samps->output_sr);
    LOG(Debug, "cloned %p to %p", cached, samp);
    LightningEvent_broadcast(samps->play_event, samp);
    return samp;
}

int
Samples_write(Samples samps,
              sample_t **buffers,
              channels_t channels,
              nframes_t frames)
{
    assert(samps);

    int i = 0;
    int chan = 0;
    int frame = 0;
    int sample_write_error = 0;

    if (!Realtime_is_processing(samps->state)) {
        return 0;
    }

    /* zero out sum buffers */

    for (chan = 0; chan < channels; chan++) {
        memset(samps->sum_bufs[chan], 0, frames * SAMPLE_SIZE);
    }

    /* add any new samples to the active list */

    i = 0;
    while (Ringbuffer_read(samps->play_buf,
                           (void *) &samps->new_sample,
                           sizeof(Sample))) {
        for ( ; i < MAX_POLYPHONY; i++) {
            if (samps->active[i] == NULL) {
                /* assign to the open sample slot and read another
                   new sample from the ring buffer */
                samps->active[i] = samps->new_sample;
                break;
            }
        }
    }

    /* write samples to output buffers */

    for (i = 0; i < MAX_POLYPHONY; i++) {
        if (samps->active[i] == NULL)
            continue;

        /* fill buffers with sample data */
        sample_write_error =                                            \
            Sample_write(samps->active[i], samps->collect_bufs, channels, frames);

        if (sample_write_error) {
            return sample_write_error;
        }

        for (chan = 0; chan < ASSUMED_CHANNELS; chan++) {
            for (frame = 0; frame < frames; frame++) {
                samps->sum_bufs[chan][frame] += samps->collect_bufs[chan][frame];
            }
        }

        if (Sample_done(samps->active[i])) {
            /* remove from the active list and free the sample */
            LightningEvent_try_broadcast(samps->free_event, samps->active[i]);
            /* Sample_free(&samps->active[i]); */
            samps->active[i] = NULL;
        }
    }

    /* copy sum buffers to output buffers */

    for (chan = 0; chan < channels; chan++) {
        for (frame = 0; frame < frames; frame++) {
            buffers[chan][frame] = samps->sum_bufs[chan][frame];
        }
    }

    return 0;
}

int
Samples_wait(Samples samps)
{
    assert(samps);
    // wait for them 1 by 1
    int i, rc;
    for (i = 0; i < MAX_POLYPHONY; i++) {
        if (samps->active[i] != NULL) {
            rc = Sample_wait(samps->active[i]);
            LOG(Debug, "Could not wait for %s", Sample_path(samps->active[i]));
            if (rc != 0) {
                return rc;
            }
        }
    }
    return 0;
}

void
Samples_free(Samples *samps)
{
    assert(samps && *samps);
    int i = 0;
    Samples s = *samps;
    LightningEvent_free(&s->play_event);
    Ringbuffer_free(&s->play_buf);
    BinTree_free(&s->cache);
    /* free auxiliary buffers */
    for (i = 0; i < ASSUMED_CHANNELS; i++) {
        FREE(s->sum_bufs[i]);
        FREE(s->collect_bufs[i]);
    }
    LightningThread_free(&s->play_thread);
    LightningThread_free(&s->free_thread);
    Realtime_free(&s->state);
    /* FREE(s->new_sample); */
    FREE(s->sum_bufs);
    FREE(s->collect_bufs);
    FREE(*samps);
}

void *
play_new_samples(void *arg)
{
    LightningThreadData data = (LightningThreadData) arg;
    LightningEvent event = data->event;
    Ringbuffer rb = data->buf;
    while (1) {
        LightningEvent_wait(event);
        Sample samp = (Sample) LightningEvent_value(event);
        if (samp != NULL) {
            LOG(Debug, "play_new_samples adding %p to the ringbuffer", samp);
            Ringbuffer_write(rb, (void *) &samp, sizeof(Sample));
        }
    }
}

void *
free_done_samples(void *arg)
{
    LightningThreadData data = (LightningThreadData) arg;
    LightningEvent event = data->event;
    while (1) {
        LightningEvent_wait(event);
        Sample samp = (Sample) LightningEvent_value(event);
        if (samp != NULL) {
            LOG(Debug, "free_done_samples freeing %p", samp);
            Sample_free(&samp);
        }
    }
}
