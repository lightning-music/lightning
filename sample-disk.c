#include <assert.h>

#include "mem.h"
#include "ringbuffer.h"
#include "sample-disk.h"
#include "thread.h"
#include "lightning.h"

struct SampleDisk {
    char *path;
};

SampleDisk
SampleDisk_init(const char *file, pitch_t pitch,
                gain_t gain, nframes_t output_samplerate)
{
    SampleDisk s;
    NEW(s);
    return s;
}

const char *
SampleDisk_path(SampleDisk samp)
{
    assert(samp);
    return samp->path;
}

nframes_t
SampleDisk_write(SampleDisk samp, sample_t **buffers, channels_t channels,
                 nframes_t frames)
{
    return 0;
}

int
SampleDisk_done(SampleDisk samp)
{
    return 1;
}

int
SampleDisk_wait(SampleDisk samp)
{
    return 0;
}

void
SampleDisk_free(SampleDisk *samp)
{
}
