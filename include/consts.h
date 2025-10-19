#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <raylib.h>
#include <stdlib.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#endif

// screen dimensions
extern const int X;
extern const int Y;
extern const int FPS;

// name of kernel file
extern const char *KERNEL_FILENAME;

// particle system
extern const int N;
extern const int MIN_CAPACITY;
extern const float R;
extern const Color color;

// physics constants
extern const float G;
extern const Vector2 g;
extern const float DAMPING;

// SPH parameters
extern const float H;           // interaction radius
extern const float D_0;         // rest density
extern const float K;           // stiffness constant
extern const float K_NEAR;      // near stiffness constant
extern const float K_SPRING;    // spring constant for elasticity
extern const float SPRING_THRESHOLD;  // resize H as threshold to remove a spring

// plasticity constants
extern const float GAMMA;       // yield ratio
extern const float ALPHA;       // plasticity constant
extern const float SIGMA;       // first viscosity constant
extern const float BETA;        // second viscosity constant

typedef struct {
    char *source;
    size_t size;
} Source;

typedef struct {
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_mem pos_mem_obj;
    cl_mem prev_pos_mem_obj;
    cl_mem vel_mem_obj;
    cl_mem is_mem_obj;
    cl_mem js_mem_obj;
    cl_mem rl_mem_obj;
} OpenCLState;

typedef struct {
    Vector2 *positions;
    Vector2 *prev_positions;
    Vector2 *velocities;
} ParticleSystem;

typedef struct {
    int *is;
    int *js;
    float *rest_lengths;
    int count;
    int capacity;
} Springs;

#endif
