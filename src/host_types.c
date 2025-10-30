#include "host_types.h"

Constants init_consts(int FPS) {
    Constants constants = {
        .X = 1000,
        .Y = 800,
        .dt = 1.f / FPS,

        .N = 1024,
        .MIN_CAPACITY = 64,
        .R = 4.f
    };

    return constants;
}

// color of particles
const Color color = GRAY;

// path to the kernel files
const char *KERNEL_PATH = "kernels/kernel.cl";

float rand_float(float low, float high) {
    return ((float)rand() / (float)(RAND_MAX)) * fabs(low - high) + low;
}
