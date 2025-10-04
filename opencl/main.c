#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define N 128

typedef struct {
    char *source;
    size_t size;
} Source;

Source read_kernel(const char *filename, int unsigned show) {
    FILE *fptr = fopen(filename, "r");

    if (!fptr) {
        fprintf(stderr, "Could not load the kernel\n");
        exit(1);
    }

    fseek(fptr, 0L, SEEK_END);
    size_t file_size = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    Source s;
    s.source = (char*)malloc(file_size);
    s.size = fread(s.source, 1, file_size, fptr);

    if (show) {
        printf("\n--------- Kernel ---------\n");
        printf("%s", s.source);
        printf("----------------------------\n");
    }

    fclose(fptr);
    return s;
}

void show_device_info(cl_device_id device, cl_uint num_devices) {
    printf("\n----------- Info -----------\n");
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

    int len = (int)n_item_sizes/8;
    size_t item_sizes[len];
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(item_sizes), &item_sizes, NULL);

    printf("Max work-items per dimension: ");
    for (int i=0; i<len; i++) {
        if (i != len - 1) printf("%zu, ", item_sizes[i]);
        else printf("%zu\n", item_sizes[i]);
    }

    size_t group_size;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &group_size, NULL);
    printf("Max work-items in a work-group: %zu\n", group_size);

    cl_ulong mem_size;
    clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &mem_size, NULL);
    printf("Global memory: %.1f GB\n", (double)mem_size / (1073741824.f));

    printf("----------------------------\n");
}

void check_error(cl_int err, const char *operation) {
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error during %s: %d\n", operation, err);
        exit(1);
    }
}

void program_log(cl_program program, cl_device_id device) {
    size_t log_len;
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_len);

    char *program_log = (char*)malloc(log_len);

    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_len, program_log, NULL);
    printf("\n------- Build log --------\n");
    printf("%s\n", program_log);
    printf("----------------------------\n\n");

    free(program_log);
}


int main() {
    float *data = (float*)malloc(sizeof(float) * N);
    float *resu = (float*)malloc(sizeof(float) * N);

    for (int i=0; i<N; i++) {
        data[i] = (float)i / (float)N;
    }

    Source s = read_kernel("kernel.cl", 1);

    cl_platform_id platform = NULL;
    cl_device_id device = NULL;
    cl_uint num_devices;

    cl_int info = clGetPlatformIDs(1, &platform, NULL);
    check_error(info, "clGetPlatformIDs");

    info = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, &num_devices);
    check_error(info, "clGetDeviceIDs");
    show_device_info(device, num_devices);

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &info);
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, 0, &info);

    // create memory buffers
    cl_mem data_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, N * sizeof(float), NULL, &info);
    cl_mem resu_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, N * sizeof(float), NULL, &info);

    // copy data to buffers
    info = clEnqueueWriteBuffer(queue, data_mem_obj, CL_TRUE, 0, N * sizeof(float), data, 0, NULL, NULL);
    check_error(info, "clEnqueueWriteBuffer");

    cl_program program = clCreateProgramWithSource(
            context,
            1,
            (const char**)&s.source,
            (const size_t*)&s.size,
            &info);

    info = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    program_log(program, device);
    check_error(info, "clBuildProgram");

    cl_kernel kernel = clCreateKernel(program, "calcSin", &info);

    // set kernel arguments
    info = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&data_mem_obj);
    check_error(info, "clSetKernelArg0");
    info = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&resu_mem_obj);
    check_error(info, "clSetKernelArg1");

    // execute kernel
    size_t global_work_size = N;
    size_t local_work_size = 8;
    info = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);
    check_error(info, "clEnqueueNDRangeKernel");

    // read results back
    info = clEnqueueReadBuffer(queue, resu_mem_obj, CL_TRUE, 0, N * sizeof(float), resu, 0, NULL, NULL);
    check_error(info, "clEnqueueReadBuffer");

    printf("Calculation complete\n");
    for (int i=0; i<10; i++) {
        printf("sin(%f) = %f\n", data[i], resu[i]);
    }

    info = clReleaseKernel(kernel);
    info = clReleaseProgram(program);
    info = clReleaseMemObject(data_mem_obj);
    info = clReleaseMemObject(resu_mem_obj);
    info = clReleaseCommandQueue(queue);
    info = clReleaseContext(context);

    free(data);
    free(resu);
    free(s.source);

    return 0;
}
