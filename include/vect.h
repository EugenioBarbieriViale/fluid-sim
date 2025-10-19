#ifndef VECT_OPERATIONS_H
#define VECT_OPERATIONS_H

#include <raylib.h>
#include <math.h>

static inline Vector2 sum(Vector2 v, Vector2 w) {
    return (Vector2){v.x + w.x, v.y + w.y};
}

static inline Vector2 diff(Vector2 v, Vector2 w) {
    return (Vector2){v.x - w.x, v.y - w.y};
}

static inline Vector2 scalar_mult(Vector2 v, float s) {
	return (Vector2){v.x * s, v.y * s};
}

static inline float length(Vector2 v) {
    return (float)sqrt(v.x*v.x + v.y*v.y);
}

#endif
