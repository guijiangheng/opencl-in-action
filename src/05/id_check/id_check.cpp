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
	auto queue = clCreateCommandQueue(context, device, 0, NULL);
	auto program = buildProgram(context, device, "id_check.cl");
	auto kernel = clCreateKernel(program, "id_check", NULL);

	float result[24];
	size_t global_offset[] = { 3, 5 };
	size_t global_size[] = { 6, 4 };
	size_t local_size[] = { 3, 2 };
	auto buffer = clCreateBuffer(
				  context, CL_MEM_WRITE_ONLY, sizeof(result), NULL, NULL);

	clSetKernelArg(kernel, 0, sizeof(buffer), &buffer);
	clEnqueueNDRangeKernel(queue, kernel, 2,
						   global_offset, global_size, local_size,
						   0, NULL, NULL);
	clEnqueueReadBuffer(queue, buffer, CL_TRUE,
						0, sizeof(result), result, 0, NULL, NULL);

	for (int i = 0; i < 24; i += 6) {
		printf("%.2f %.2f %.2f %.2f %.2f %.2f\n",
			   result[i], result[i + 1], result[i + 2],
			   result[i + 3], result[i + 4], result[i + 5]);
	}

	clReleaseMemObject(buffer);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseDevice(device);
	clReleaseContext(context);

	return 0;
}