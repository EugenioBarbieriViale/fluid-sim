#include "parallel.h"

static KernelSource read_file(const char *file_name, int unsigned show) {
    FILE *fptr = fopen(file_name, "r");

    if (!fptr) {
        fprintf(stderr, "Could not load the kernel\n");
        exit(1);
    }

    fseek(fptr, 0L, SEEK_END);
    size_t file_size = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    KernelSource s;
    s.source = (char*)malloc(file_size + 1);

    if (!s.source) {
        fprintf(stderr, "Could not allocate memory for the kernel\n");
        exit(1);
    }

    s.size = fread(s.source, 1, file_size, fptr);
    s.source[file_size] = '\0';

    if (show) {
        printf("\n------- Input file -------\n");
        printf("%s", s.source);
        printf("----------------------------\n");
    }

    fclose(fptr);
    return s;
}

static void show_device_info(cl_device_id device, cl_uint num_devices) {
    printf("\n------ Device info -------\n");
    printf("Number of devices found: %u\n", num_devices);

    cl_device_type device_type;
    clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);
    printf("Device type: %s\n", (device_type == CL_DEVICE_TYPE_GPU) ? "GPU" : "CPU");

    cl_uint compute_units;
    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &compute_units, NULL);
    printf("Max compute units: %u\n", compute_units);

    cl_uint work_dims;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &work_dims, NULL);
    printf("Max work-item dimensions: %u\n", work_dims);

    size_t n_item_sizes;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, 0, NULL, &n_item_sizes);

    int len = (int)n_item_sizes / sizeof(size_t);
    size_t item_sizes[len];
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(item_sizes), &item_sizes, NULL);

    printf("Max work group size per dimension: ");
    for (int i=0; i<len; i++) {
        if (i != len - 1) printf("%zu, ", item_sizes[i]);
        else printf("%zu\n", item_sizes[i]);
    }

    size_t group_size;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &group_size, NULL);
    printf("Max work group size: %zu\n", group_size);

    cl_ulong mem_size;
    clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &mem_size, NULL);
    printf("Global memory: %.1f GB\n", (double)mem_size / (1073741824.f));

    printf("----------------------------\n");
}

static void show_kernel_info(cl_device_id device, cl_kernel kernel, KernelConfig config) {
    size_t work_group_size;
    clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &work_group_size, NULL);
    printf("\n------ Kernel info -------\n");
    printf("Kernel-specific max work group size: %zu\n", work_group_size);
    printf("Global group size is set to: %zu\n", config.global_work_size);
    printf("Local group size is set to: %zu\n", config.local_work_size);
    printf("----------------------------\n\n");
}

static void check_error(cl_int err, const char *operation) {
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error during %s: %d\n", operation, err);
        exit(1);
    }
}

static void program_log(cl_program program, cl_device_id device) {
    size_t log_len;
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_len);

    char *program_log = (char*)malloc(log_len + 1);

    if (!program_log) {
        fprintf(stderr, "Could not allocate memory for program log\n");
        exit(1);
    }

    program_log[log_len] = '\0';

    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_len, program_log, NULL);
    printf("\n------- Build log --------\n");
    printf("%s\n", program_log);
    printf("----------------------------\n");

    free(program_log);
}

OpenCLState init_opencl(Constants consts, int show_file) {

    OpenCLState cl_state;

    cl_platform_id platform = NULL;
    cl_state.device = NULL;
    cl_uint num_devices;

    cl_int info = clGetPlatformIDs(1, &platform, NULL);
    check_error(info, "clGetPlatformIDs");

    info = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &cl_state.device, &num_devices);
    check_error(info, "clGetDeviceIDs");
    show_device_info(cl_state.device, num_devices);

    cl_state.config.global_work_size = consts.N;

    size_t group_size;
    clGetDeviceInfo(cl_state.device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &group_size, NULL);
    if (group_size > (size_t)consts.N) 
        group_size = (size_t)consts.N;
    cl_state.config.local_work_size = group_size;

    cl_state.context = clCreateContext(NULL, 1, &cl_state.device, NULL, NULL, &info);
    check_error(info, "clCreateContext");
    cl_state.queue = clCreateCommandQueueWithProperties(cl_state.context, cl_state.device, 0, &info);
    check_error(info, "clCreateCommandQueueWithProperties");

    cl_state.sys_mem_obj = clCreateBuffer(cl_state.context, CL_MEM_READ_WRITE, sizeof(ParticleSystem), NULL, &info);
    check_error(info, "clCreateBuffer");
    cl_state.sprs_mem_obj = clCreateBuffer(cl_state.context, CL_MEM_READ_WRITE, sizeof(Springs), NULL, &info);
    check_error(info, "clCreateBuffer");
    // cl_mem consts_mem_obj = clCreateBuffer(cl_state.context, CL_MEM_READ_ONLY, sizeof(Constants), NULL, &info);
    // check_error(info, "clCreateBuffer");

    KernelSource s = read_file(KERNEL_PATH, show_file);

    cl_state.program = clCreateProgramWithSource(
            cl_state.context,
            1,
            (const char**)&s.source,
            (const size_t*)&s.size,
            &info);
    check_error(info, "clCreateProgramWithKernelSource");

    info = clBuildProgram(cl_state.program, 1, &cl_state.device, NULL, NULL, NULL);
    program_log(cl_state.program, cl_state.device);
    check_error(info, "clBuildProgram");

    cl_state.kernel = clCreateKernel(cl_state.program, "computeNextState", &info);
    check_error(info, "clCreateKernel");
    show_kernel_info(cl_state.device, cl_state.kernel, cl_state.config);

    info = clSetKernelArg(cl_state.kernel, 0, sizeof(cl_mem), (void*)&cl_state.sys_mem_obj);
    check_error(info, "clSetKernelArg0");
    info = clSetKernelArg(cl_state.kernel, 1, sizeof(cl_mem), (void*)&cl_state.sprs_mem_obj);
    check_error(info, "clSetKernelArg1");
    info = clSetKernelArg(cl_state.kernel, 2, sizeof(Constants), &consts);
    check_error(info, "clSetKernelArg2");

    free(s.source);

    return cl_state;
}

void release_opencl(OpenCLState *cl_state) {
    cl_int info;

    info = clReleaseKernel(cl_state->kernel);
    check_error(info, "clReleaseKernel");
    info = clReleaseProgram(cl_state->program);
    check_error(info, "clReleaseProgram");

    info = clReleaseMemObject(cl_state->sys_mem_obj);
    check_error(info, "clReleaseMemObject");
    info = clReleaseMemObject(cl_state->sprs_mem_obj);
    check_error(info, "clReleaseMemObject");

    info = clReleaseCommandQueue(cl_state->queue);
    check_error(info, "clReleaseCommandQueue");
    info = clReleaseContext(cl_state->context);
    check_error(info, "clReleaseContext");
}

void parallel_compute(OpenCLState *cl_state, ParticleSystem *sys, Springs *sprs) {
    cl_int info;

    // copy data to buffers
    info = clEnqueueWriteBuffer(cl_state->queue, cl_state->sys_mem_obj, CL_TRUE, 0, sizeof(ParticleSystem), sys, 0, NULL, NULL);
    check_error(info, "clEnqueueWriteBuffer");
    info = clEnqueueWriteBuffer(cl_state->queue, cl_state->sprs_mem_obj, CL_TRUE, 0, sizeof(Springs), sprs, 0, NULL, NULL);
    check_error(info, "clEnqueueWriteBuffer");

    // execute kernel
    size_t GLOBAL_WORK_SIZE = cl_state->config.global_work_size;
    size_t LOCAL_WORK_SIZE = cl_state->config.local_work_size;

    info = clEnqueueNDRangeKernel(cl_state->queue, cl_state->kernel, 1, NULL, &GLOBAL_WORK_SIZE, &LOCAL_WORK_SIZE, 0, NULL, NULL);
    check_error(info, "clEnqueueNDRangeKernel");

    // read results back
    info = clEnqueueReadBuffer(cl_state->queue, cl_state->sys_mem_obj, CL_TRUE, 0, sizeof(ParticleSystem), sys, 0, NULL, NULL);
    check_error(info, "clEnqueueReadBuffer");
    info = clEnqueueReadBuffer(cl_state->queue, cl_state->sprs_mem_obj, CL_TRUE, 0, sizeof(Springs), sprs, 0, NULL, NULL);
    check_error(info, "clEnqueueReadBuffer");
}
