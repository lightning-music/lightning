#include <assert.h>
#include <types.h>
#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "mem.h"
#include "sample.h"
#include "sample-ram.h"

static void
not_implemented(void)
{
    fprintf(stderr, "SampleDisk not implemented\n");
    exit(EXIT_FAILURE);
}

/**
 * Read a sound file, de-interleave the channels if necessary,
 * perform sample rate conversion, then cache this data in memory.
 */
Sample
Sample_init(const char *file, pitch_t pitch,
            gain_t gain, nframes_t output_sr)
{
    Sample s;
    NEW(s);
    switch (SAMPLE_TYPE) {
    case SampleType_RAM: {
        s->ram = SampleRam_init(file, pitch, gain, output_sr);
        break; }
    case SampleType_DISK: {
        not_implemented();
        break; }
    }
    return s;
}

Sample
Sample_clone(Sample orig, pitch_t pitch, gain_t gain, nframes_t output_sr)
{
    Sample s;
    NEW(s);
    switch (SAMPLE_TYPE) {
    case SampleType_RAM: {
        s->ram = SampleRam_clone(orig->ram, pitch, gain, output_sr);
        break; }
    case SampleType_DISK: {
        not_implemented();
        break; }
    }
    return s;
}

int
Sample_isnull(Sample samp)
{
    if (samp == NULL)
        return 1;

    switch(SAMPLE_TYPE) {
    case SampleType_RAM:
        return samp->ram == NULL;
    case SampleType_DISK:
        not_implemented();
    }
}

/**
 * Path to loaded file.
 */
const char *
Sample_path(Sample samp)
{
    assert(samp);
    switch(SAMPLE_TYPE) {
    case SampleType_RAM: {
        return SampleRam_path(samp->ram); }
    case SampleType_DISK: not_implemented();
    }
}

nframes_t
Sample_write(Sample samp, sample_t **buffers, channels_t channels,
             nframes_t frames)
{
    assert(samp);
    switch(SAMPLE_TYPE) {
    case SampleType_RAM: {
        return SampleRam_write(samp->ram, buffers, channels, frames); }
    case SampleType_DISK: not_implemented();
    }
}

int
Sample_done(Sample samp)
{
    assert(samp);
    switch (SAMPLE_TYPE) {
    case SampleType_RAM: {
        return SampleRam_done(samp->ram); }
    case SampleType_DISK: not_implemented();
    }
}

int
Sample_wait(Sample samp)
{
    assert(samp);
    switch (SAMPLE_TYPE) {
    case SampleType_RAM: {
        return SampleRam_wait(samp->ram); }
    case SampleType_DISK: not_implemented();
    }
}

/**
 * Free resources associated with this sample.
 */
void
Sample_free(Sample *samp)
{
    assert(samp && *samp);
    switch (SAMPLE_TYPE) {
    case SampleType_RAM: {
        SampleRam_free(&(*samp)->ram);
        break; }
    case SampleType_DISK: not_implemented();
    }
    FREE(*samp);
}
