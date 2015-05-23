#ifndef SAMPLE_DISK_H_INCLUDED
#define SAMPLE_DISK_H_INCLUDED

#include "lightning.h"

typedef struct SampleDisk *SampleDisk;

SampleDisk
SampleDisk_init(const char *file, pitch_t pitch,
                gain_t gain, nframes_t output_samplerate);

const char *
SampleDisk_path(SampleDisk samp);

nframes_t
SampleDisk_write(SampleDisk samp, sample_t **buffers, channels_t channels,
                 nframes_t frames);

int
SampleDisk_done(SampleDisk samp);

int
SampleDisk_wait(SampleDisk samp);

void
SampleDisk_free(SampleDisk *samp);

#endif
