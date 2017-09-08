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

void CL_CALLBACK read_complete(cl_event e, cl_int status, void* data) {
	auto float_data = (float*)data;
	printf("%4.2f %4.2f %4.2f %4.2f\n",
		float_data[0], float_data[1], float_data[2], float_data[3]);
}

int main() {
	auto platform = getAMDPlatform();
	cl_device_id device;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	auto context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
	auto queue = clCreateCommandQueue(context, device, 0, NULL);
	auto program = buildProgram(context, device, "user_event.cl");
	auto kernel = clCreateKernel(program, "user_event", NULL);

	float data[4] = { 1.0f, 2.0f, 3.0f, 4.0f };
	auto buffer = clCreateBuffer(context,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(data), data, NULL);
	clSetKernelArg(kernel, 0, sizeof(buffer), &buffer);

	auto user_event = clCreateUserEvent(context, NULL);
	cl_event kernel_event, read_event;
	clEnqueueTask(queue, kernel, 1, &user_event, &kernel_event);
	clEnqueueReadBuffer(queue, buffer, CL_FALSE,
		0, sizeof(data), data, 1, &kernel_event, &read_event);
	clSetEventCallback(read_event, CL_COMPLETE, &read_complete, data);

	_sleep(1);
	printf("Old data: %4.2f %4.2f %4.2f %4.2f\n",
		data[0], data[1], data[2], data[3]);
	printf("Press NETER to continue.");
	getchar();

	clSetUserEventStatus(user_event, CL_SUCCESS);

	clReleaseMemObject(buffer);
	clReleaseEvent(user_event);
	clReleaseEvent(kernel_event);
	clReleaseEvent(read_event);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseDevice(device);
	clReleaseContext(context);

	return 0;
}