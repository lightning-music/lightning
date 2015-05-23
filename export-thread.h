#ifndef EXPORT_THREAD_H_INCLUDED
#define EXPORT_THREAD_H_INCLUDED

#include "lightning.h"

typedef struct ExportThread *ExportThread;

/**
 * Start a thread for writing to a file
 *
 * @param  output_sr - Output sample rate
 * @param  channels - Output channels
 *
 * @return ExportThread structure
 */
ExportThread
ExportThread_create(nframes_t output_sr, channels_t channels);

/**
 * Make the export thread wait for data
 *
 * @param  thread - ExportThread, can not be NULL
 * @param  bufs - Sample buffers
 * @param  frames - Number of frames to write. Each buffer in @a bufs must contain this many frames.
 */
nframes_t
ExportThread_write(ExportThread thread, sample_t **bufs, nframes_t frames);

/**
 * Start exporting to a file.
 * Fails if the export thread is already exporting.
 *
 * @param  thread - ExportThread, can not be NULL
 * @param  file - file to save exported data in
 *
 * @return 0 on success, nonzero on failure
 */
int
ExportThread_start(ExportThread thread, const char *file);

/**
 * Stop exporting to a file.
 * Does nothing if the export thread is not currently exporting.
 *
 * @param  thread - ExportThread
 *
 * @return 0 on success, nonzero on failure
 */
int
ExportThread_stop(ExportThread thread);

/**
 * Destroy an export thread
 */
void
ExportThread_free(ExportThread *thread);

#endif
