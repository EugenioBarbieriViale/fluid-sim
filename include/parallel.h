#ifndef PARALLEL_H
#define PARALLEL_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "springs.h"

OpenCLState init_opencl(int);
void release_opencl(OpenCLState*);
void parallel_compute(OpenCLState*, ParticleSystem*, Springs*);

#endif
