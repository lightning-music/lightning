#ifndef LIGHTNING_H_INCLUDED
#define LIGHTNING_H_INCLUDED

#include "types.h"

/**
 * Main lightning data structure.
 *
 * All functions in this interface will fail if
 * passed NULL instead of a properly initialized
 * Lightning instance.
 *
 * Typically an application will use a Lightning
 * instance for its entire life, so you may never need
 * to call Lightning_free()
 */
typedef struct Lightning *Lightning;

/**
 * Entrypoint for liblightning.
 *
 * The server also broadcasts messages over OSC/websocket
 * that provide clients a way to know what it is doing
 * internally.
 */
Lightning
Lightning_init();

/**
 * Connect lightning to a pair of JACK sinks.
 *
 * @param lightning Lightning instance
 * @param ch1 first JACK output channel
 * @param ch2 second JACK output channel
 *
 * @return 0 (success), nonzero (failure)
 */
int
Lightning_connect_to(Lightning lightning, const char *ch1, const char *ch2);

/**
 * Add a directory for lighting to search for audio files.
 *
 * @param lightning Lightning instance
 * @param dir Directory to add to lightning's search path
 *
 * @return 0 (success), nonzero (failure)
 */
int
Lightning_add_dir(Lightning lightning, const char *dir);

/**
 * Play a sample.
 *
 * @param lightning Lightning instance
 * @param file Audio file to play
 * @param pitch Playback speed. `1.0` is normal speed, `0.5` half speed, and so on.
 * @param gain Gain [0.0, 1.0]
 *
 * @return 0 success, nonzero failure
 */
int
Lightning_play_sample(Lightning lightning, const char *file,
                      pitch_t pitch, gain_t gain);

/**
 * Start exporting to an audio file
 *
 * @param lightning Lightning instance
 *
 * @return 0 success, nonzero failure
 */
int
Lightning_export_start(Lightning lightning, const char *file);

/**
 * If currently exporting, stop.
 *
 * @param lightning Lightning instance
 *
 * @return 0 success, nonzero failure
 */
int
Lightning_export_stop(Lightning lightning);

/**
 * Free the system resources associated with a Lightning instance.
 *
 * @param lightning pointer to a Lightning instance
 */
void
Lightning_free(Lightning *lightning);

#endif
