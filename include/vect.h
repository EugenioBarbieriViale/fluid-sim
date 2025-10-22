#ifndef VECT_OPERATIONS_H
#define VECT_OPERATIONS_H

#include "consts.h"
#include <math.h>

static inline vector2 sum(vector2 v, vector2 w) {
    return (vector2){v.x + w.x, v.y + w.y};
}

static inline vector2 diff(vector2 v, vector2 w) {
    return (vector2){v.x - w.x, v.y - w.y};
}

static inline vector2 scalar_mult(vector2 v, float s) {
	return (vector2){v.x * s, v.y * s};
}

static inline float length(vector2 v) {
    return (float)sqrt(v.x*v.x + v.y*v.y);
}

#endif
