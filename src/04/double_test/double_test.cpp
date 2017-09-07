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
						int n, cl_device_id* devices,
						const char* filename,
						const char* options = NULL) {
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
	clBuildProgram(program, n, devices, options, NULL, NULL);
	free(source);

	return program;
}

int main() {
	auto platform = getAMDPlatform();
	cl_device_id device;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
	cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

	size_t ext_size;
	clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, 0, NULL, &ext_size);
	auto ext_data = (char*)malloc(ext_size + 1);
	ext_data[ext_size] = '\0';
	clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, ext_size, ext_data, NULL);
	const char* options = NULL;
	if (strstr(ext_data, "cl_khr_fp64") != NULL) {
		printf("The cl_khr_fp64 extension is supported.\n");
		options = "-DFP_64";
	} else
		printf("The cl_khr_fp64 extension is not supported.\n");
	free(ext_data);

	cl_program program = buildProgram(context, 1, &device, "double_test.cl", options);
	cl_kernel kernel = clCreateKernel(program, "double_test", NULL);
	auto buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float), NULL, NULL);
	float a = 4, b = 2, c;
	clSetKernelArg(kernel, 0, sizeof(float), &a);
	clSetKernelArg(kernel, 1, sizeof(float), &b);
	clSetKernelArg(kernel, 2, sizeof(buffer), &buffer);
	clEnqueueTask(queue, kernel, 0, NULL, NULL);
	clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, sizeof(float), &c, 0, NULL, NULL);
	printf("The kernel result is: %f\n", c);

	clReleaseMemObject(buffer);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseDevice(device);
	clReleaseContext(context);

	return 0;
}