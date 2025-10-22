#ifndef PARALLEL_H
#define PARALLEL_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#endif

#include "host_types.h"

OpenCLState init_opencl(Constants consts, int show_file);
void release_opencl(OpenCLState*);
void parallel_compute(OpenCLState*, ParticleSystem*, Springs*);

#endif
