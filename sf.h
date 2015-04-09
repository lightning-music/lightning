#ifndef SF_H_INCLUDED
#define SF_H_INCLUDED

#include <types.h>
#include <sndfile.h>

typedef enum {
    /* libsndfile formats */
    SF_FMT_WAV = SF_FORMAT_WAV,
    SF_FMT_AIFF = SF_FORMAT_AIFF,
    SF_FMT_FLAC = SF_FORMAT_FLAC,
    SF_FMT_OGG = SF_FORMAT_OGG
} SF_FMT;

typedef struct SF *SF;

/**
 * Open a sound file for reading.
 *
 * @param file - The path to a file
 * @param mode - Open mode (read, write, or read/write)
 */
SF
SF_open_read(const char *file);

/**
 * Open a sound file for writing.
 *
 * @param file - The path to the new file.
 * @param channels - The number of channels to write.
 * @param samplerate - The sample rate of the file.
 * @param format - The format of the file.
 *
 * @return struct SF *
 */
SF
SF_open_write(const char *file, channels_t channels,
              nframes_t samplerate, SF_FMT format);

/**
 * Get the channels of a sound file.
 */
channels_t
SF_channels(SF sf);

/**
 * Get the frames of a sound file.
 */
nframes_t
SF_frames(SF sf);

/**
 * Get the sample rate of a sound file.
 */
nframes_t
SF_samplerate(SF sf);

/**
 * Read @a frames frames from a sound file, and
 * store them in @a buf. @a buf must be large enough
 * to hold at least @a frames frames of data.
 *
 * @return Number of frames actually read
 */
nframes_t
SF_read(SF sf, sample_t *buf, nframes_t frames);

/**
 * Write @a frames frames of data to a sound file.
 */
nframes_t
SF_write(SF sf, sample_t *buf, nframes_t frames);

/**
 * Get a string that describes the last error that occured
 * with the given SF object.
 */
const char *
SF_strerror(SF sf);

/**
 * Close the soundfile.
 */
void
SF_close(SF *sf);

#endif
