#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CL/cl.h>

cl_platform_id getAMDPlatform() {
	cl_uint n;
	clGetPlatformIDs(0, NULL, &n);
	auto platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * n);
	clGetPlatformIDs(n, platforms, NULL);

	char buffer[1024];
	cl_platform_id platform;
	for (int i = 0; i < n; ++i) {
		clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 1024, buffer, NULL);
		if (strstr(buffer, "AMD") != NULL) {
			platform = platforms[i];
			break;
		}
	}
	free(platforms);

	return platform;
}

cl_program buildProgram(cl_context context,
	cl_device_id device, const char* filename) {
	FILE* file = fopen(filename, "rb");
	fseek(file, 0, SEEK_END);
	size_t length = ftell(file);
	char* source = (char*)malloc(length + 1);
	source[length] = '\0';
	rewind(file);
	fread(source, sizeof(char), length, file);
	fclose(file);

	auto program = clCreateProgramWithSource(
		context, 1, (const char**)&source, &length, NULL);
	free(source);

	auto err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	if (err < 0) {
		size_t length;
		char log[2048];
		clGetProgramBuildInfo(program, device,
			CL_PROGRAM_BUILD_LOG, 2048, log, &length);
		log[length] = '\0';
		printf("Build log:\n%s\n", log);
	}

	return program;
}

int main() {
	auto platform = getAMDPlatform();
	cl_device_id device;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	auto context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
	auto queue = clCreateCommandQueue(
		context, device, CL_QUEUE_PROFILING_ENABLE, NULL);
	auto program = buildProgram(context, device, "profile_items.cl");
	auto kernel = clCreateKernel(program, "profile_items", NULL);

	const int NUM_INTS = 4096;
	const int NUM_ITERS = 2000;
	const size_t NUM_ITEMS = 512;

	int data[NUM_INTS];
	auto buffer = clCreateBuffer(context,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(data), data, NULL);
	clSetKernelArg(kernel, 0, sizeof(buffer), &buffer);
	clSetKernelArg(kernel, 1, sizeof(int), &NUM_INTS);

	cl_ulong time_start, time_end;
	cl_ulong time_total = 0;
	cl_event profile_event;
	for (int i = 0; i < NUM_ITERS; ++i) {
		clEnqueueNDRangeKernel(queue, kernel,
			1, NULL, &NUM_ITEMS, NULL, 0, NULL, &profile_event);
		clFinish(queue);
		clGetEventProfilingInfo(profile_event,
			CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
		clGetEventProfilingInfo(profile_event,
			CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
		time_total += time_end - time_start;
	}
	printf("Average time = %lu\n", time_total / NUM_ITERS);

	clReleaseMemObject(buffer);
	clReleaseEvent(profile_event);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseDevice(device);
	clReleaseContext(context);

	return 0;
}