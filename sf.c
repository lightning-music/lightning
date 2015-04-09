#include <assert.h>
#include <types.h>
#include <sndfile.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "mem.h"
#include "sf.h"

typedef enum {
    SF_MODE_READ,
    SF_MODE_WRITE
} SF_MODE;

struct SF {
    SNDFILE *sfp;
    nframes_t frames;
    channels_t channels;
    nframes_t samplerate;
    SF_MODE mode;
};

SF
SF_open_read(const char *file)
{
    SF sf;
    NEW(sf);

    SF_INFO sfinfo;
    sf->sfp = sf_open(file, SFM_READ, &sfinfo);

    if (sf->sfp == NULL) {
        LOG(Error, "could not open %s: %s", file, sf_strerror(sf->sfp));
        return NULL;
    }

    sf->channels = sfinfo.channels;
    sf->frames = sfinfo.frames;
    sf->samplerate = sfinfo.samplerate;
    sf->mode = SF_MODE_READ;

    return sf;
}

SF
SF_open_write(const char *file, channels_t channels,
              nframes_t samplerate, SF_FMT format)
{
    SF sf;
    NEW(sf);

    SF_INFO sfinfo;
    int sndfile_format;

    sf->frames = 0;
    sf->mode = SF_MODE_WRITE;
    sfinfo.channels = sf->channels = channels;
    sfinfo.samplerate = sf->samplerate = samplerate;

    switch (format) {
    case SF_FMT_WAV:  { sndfile_format = SF_FORMAT_WAV;  break; }
    case SF_FMT_AIFF: { sndfile_format = SF_FORMAT_AIFF; break; }
    case SF_FMT_FLAC: { sndfile_format = SF_FORMAT_FLAC; break; }
    case SF_FMT_OGG:  { sndfile_format = SF_FORMAT_OGG;  break; }
    default:            sndfile_format = SF_FORMAT_WAV;
    }

    sndfile_format |= SF_FORMAT_FLOAT;
    sfinfo.format = sndfile_format;
    sf->sfp = sf_open(file, SFM_WRITE, &sfinfo);

    if (sf->sfp == NULL) {
        LOG(Error, "could not open %s: %s", file, sf_strerror(sf->sfp));
        return NULL;
    }

    return sf;
}

channels_t
SF_channels(SF sf)
{
    assert(sf);
    return sf->channels;
}

nframes_t
SF_frames(SF sf)
{
    assert(sf);
    return sf->frames;
}

nframes_t
SF_samplerate(SF sf)
{
    assert(sf);
    return sf->samplerate;
}

nframes_t
SF_read(SF sf, sample_t *buf, nframes_t frames)
{
    assert(sf);
    return (nframes_t) sf_readf_float(sf->sfp, buf, frames);
}

nframes_t
SF_write(SF sf, sample_t *buf, nframes_t frames)
{
    assert(sf);
    return (nframes_t) sf_writef_float(sf->sfp, buf, frames);
}

const char *
SF_strerror(SF sf)
{
    assert(sf);
    return sf_strerror(sf->sfp);
}

void
SF_close(SF *sf)
{
    assert(sf && *sf);
    sf_close((*sf)->sfp);
    FREE(*sf);
}
