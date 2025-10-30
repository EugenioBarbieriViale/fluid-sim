#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <raylib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#endif


// color of particles
extern const Color color;

// path to kernel and header for kernel
extern const char *KERNEL_PATH;


typedef struct {
    int X;
    int Y;
    float dt;

    int N;
    int MIN_CAPACITY;
    float R;
} Constants;

Constants init_consts(int);

typedef struct {
    float x;
    float y;
} vector2;

typedef struct {
    vector2 *positions;
    vector2 *prev_positions;
    vector2 *velocities;
} ParticleSystem;

typedef struct {
    int *is;
    int *js;
    float *rest_lengths;
    int count;
    int capacity;
} Springs;

typedef struct {
    char *source;
    size_t size;
} KernelSource;

typedef struct {
    size_t n_dims;
    size_t *global_work_size;
    size_t *local_work_size;
} KernelConfig;

typedef struct {
    KernelConfig config;

    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    cl_mem sys_mem_obj;
    cl_mem sprs_mem_obj;
} OpenCLState;

float rand_float(float, float); // put directly in main.c 

#endif
