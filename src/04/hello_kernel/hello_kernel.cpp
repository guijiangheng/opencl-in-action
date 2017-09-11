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

int main() {
	auto platform = getAMDPlatform();
	cl_device_id device;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
	cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

	FILE* source_file = fopen("hello_kernel.cl", "rb");
	fseek(source_file, 0, SEEK_END);
	size_t length = ftell(source_file);
	char* source = (char*)malloc(length + 1);
	source[length] = '\0';
	rewind(source_file);
	fread(source, sizeof(char), length, source_file);
	fclose(source_file);

	cl_program program = clCreateProgramWithSource(
		context, 1, (const char**)&source, &length, NULL);
	clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	cl_kernel kernel = clCreateKernel(program, "hello_kernel", NULL);

	free(source);

	char msg[16];
	cl_mem msg_buffer = clCreateBuffer(
		context, CL_MEM_WRITE_ONLY, sizeof(msg), NULL, NULL);
	clSetKernelArg(kernel, 0, sizeof(msg_buffer), &msg_buffer);
	clEnqueueTask(queue, kernel, 0, NULL, NULL);
	clEnqueueReadBuffer(queue, msg_buffer, CL_TRUE,
		0, sizeof(msg), msg, 0, NULL, NULL);

	printf("Kernel output: %s \n", msg, strlen(msg));

	clReleaseMemObject(msg_buffer);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseDevice(device);
	clReleaseContext(context);

	return 0;
}
