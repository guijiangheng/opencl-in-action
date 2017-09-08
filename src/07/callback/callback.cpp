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

void CL_CALLBACK kernel_complete(cl_event e, cl_int status, void* data) {
	printf("%s", (char*)data);
}

void CL_CALLBACK read_complete(cl_event e, cl_int status, void* data) {
	auto buffer_data = (float*)data;
	bool check = true;
	for (int i = 0; i < 4096; ++i) {
		if (buffer_data[i] != 5.0f) {
			check = false;
			break;
		}
	}
	if (check)
		printf("The data has been initialized successfully.\n");
	else
		printf("The data has not been initialized successfully.\n");
}

int main() {
	auto platform = getAMDPlatform();
	cl_device_id device;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	auto context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
	auto queue = clCreateCommandQueue(context, device, 0, NULL);
	auto program = buildProgram(context, device, "callback.cl");
	auto kernel = clCreateKernel(program, "callback", NULL);

	float data[4096];
	auto buffer = clCreateBuffer(
		context, CL_MEM_WRITE_ONLY, sizeof(data), NULL, NULL);
	clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer);

	cl_event kernel_event;
	clEnqueueTask(queue, kernel, 0, NULL, &kernel_event);
	cl_event read_event;
	clEnqueueReadBuffer(queue, buffer, CL_FALSE,
		0, sizeof(data), data, 0, NULL, &read_event);

	char* kernel_msg = "The kernel finish successfully.\n";
	clSetEventCallback(kernel_event, CL_COMPLETE, &kernel_complete, kernel_msg);
	clSetEventCallback(read_event, CL_COMPLETE, &read_complete, data);

	clReleaseMemObject(buffer);
	clReleaseEvent(kernel_event);
	clReleaseEvent(read_event);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseDevice(device);
	clReleaseContext(context);

	return 0;
}