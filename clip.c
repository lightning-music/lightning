#include <types.h>

#include "clip.h"

inline sample_t
clip(sample_t val, sample_t min, sample_t max) {
    return val < min ? min : (val > max ? max : val);
}
