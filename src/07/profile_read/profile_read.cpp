#define _CRT_SECURE_NO_WARNINGS
#define PROFILE_READ

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
	auto program = buildProgram(context, device, "profile_read.cl");
	auto kernel = clCreateKernel(program, "profile_read", NULL);

	const int NUM_BYTES = 131072;
	const int NUM_ITER = 2000;
	char data[NUM_BYTES];
	auto buffer = clCreateBuffer(
		context, CL_MEM_WRITE_ONLY, sizeof(data), NULL, NULL);
	clSetKernelArg(kernel, 0, sizeof(buffer), &buffer);
	int n = NUM_BYTES / 16;
	clSetKernelArg(kernel, 1, sizeof(n), &n);

	cl_event profile_event;
	cl_ulong start_time, end_time;
	cl_ulong total_time = 0;
	for (int i = 0; i < NUM_ITER; ++i) {
		clEnqueueTask(queue, kernel, 0, NULL, NULL);
#ifdef PROFILE_READ
		clEnqueueReadBuffer(queue, buffer, CL_TRUE,
			0, sizeof(data), data, 0, NULL, &profile_event);
#else
		auto mapped_memory = clEnqueueMapBuffer(queue, buffer, CL_TRUE,
			CL_MAP_READ, 0, sizeof(data), 0, NULL, &profile_event, NULL);
		memcpy(data, mapped_memory, sizeof(data));
#endif
		clGetEventProfilingInfo(profile_event, CL_PROFILING_COMMAND_START,
			sizeof(start_time), &start_time, NULL);
		clGetEventProfilingInfo(profile_event, CL_PROFILING_COMMAND_END,
			sizeof(end_time), &end_time, NULL);
		total_time += start_time - end_time;
#ifndef PROFILE_READ
		clEnqueueUnmapMemObject(queue, buffer, mapped_memory, 0, NULL, NULL);
#endif
	}

#ifdef PROFILE_READ
	printf("Average read time: %lu\n", total_time / NUM_ITER);
#else
	printf("Average map time: %lu\n", total_time / NUM_ITER);
#endif

	clReleaseMemObject(buffer);
	clReleaseEvent(profile_event);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseDevice(device);
	clReleaseContext(context);

	return 0;
}