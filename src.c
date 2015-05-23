#include <assert.h>
#include <samplerate.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "lightning.h"
#include "mem.h"
#include "src.h"

struct SRC {
    SRC_STATE *state;
};

SRC
SRC_init() {
    SRC src;
    NEW(src);
    /* initialize SRC_STATE */
    int error;
    src->state = src_new(SRC_SINC_FASTEST, 1, &error);
    if (src->state == NULL) {
        fprintf(stderr, "Could not initialize sample rate converter: %s\n",
                src_strerror(error));
        exit(EXIT_FAILURE);
    }
    return src;
}

int
SRC_process(SRC src,
            double src_ratio,
            AudioData data,
            /* frames used from input buffers */
            nframes_t *input_frames_used,
            /* frames generated in output buffers */
            nframes_t *output_frames_gen,
            /* flag that will be set to non-zero if we
               exhaust the input buffer */
            int *end) {
    assert(src);
    SRC_DATA src_data;
    int error;

    src_data.src_ratio = src_ratio;

    if (data.input_frames < data.output_frames) {
        src_data.end_of_input = 1;
        *end = 1;
    } else {
        src_data.end_of_input = 0;
        *end = 0;
    }

    /* convert audio data */
    src_data.input_frames = data.input_frames;
    src_data.output_frames = data.output_frames;
    src_data.data_in = data.input;
    src_data.data_out = data.output;
    error = src_process(src->state, &src_data);

    if (error) {
        return error;
    }

    *input_frames_used = src_data.input_frames_used;
    *output_frames_gen = src_data.output_frames_gen;

    if (*output_frames_gen == 0 && src_data.end_of_input) {
        *end = 1;
    } else {
        *end = 0;
    }

    return 0;
}

const char *
SRC_strerror(int error) {
    return src_strerror(error);
}

void
SRC_free(SRC *src) {
    assert(src && *src);
    src_delete((*src)->state);
    FREE(*src);
}
