#include "host_types.h"

// screen dimensions
const int X = 1000;
const int Y = 800;
const int FPS = 60;

// number of particles
const int N = 1000;
const int MIN_CAPACITY = 64;
const float R = 4.f;

// color of particles
const Color color = GRAY;

// path to the kernel files
const char *KERNEL_PATH = "kernels/fluid.cl";

// global work size
const size_t GLOBAL_WORK_SIZE = N;

// local work size is set to the maximum the device supports

// size_t LOCAL_WORK_SIZE = 64;
const size_t LOCAL_WORK_SIZE = 1;

float rand_float(float low, float high) {
    return ((float)rand() / (float)(RAND_MAX)) * fabs(low - high) + low;
}
