#include <stdio.h>
#include <CL/cl.h>

int main() {
	cl_platform_id platform;
	clGetPlatformIDs(1, &platform, NULL);
	cl_device_id device;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
	cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

	float full_matrix[80];
	float zero_matrix[80];
	for (int i = 0; i < 80; ++i) {
		full_matrix[i] = i;
		zero_matrix[i] = 0;
	}

	cl_mem matrix_buffer = clCreateBuffer(
			context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			sizeof(full_matrix), full_matrix, NULL);

	size_t buffer_origin[3] = { 5 * sizeof(float), 3, 0 };
	size_t buffer_region[3] = { 4 * sizeof(float), 4, 1 };
	size_t host_origin[3] = { 1 * sizeof(float), 1, 0 };
	clEnqueueReadBufferRect(
			queue, matrix_buffer, CL_TRUE,
			buffer_origin, host_origin, buffer_region,
			10 * sizeof(float), 0, 10 * sizeof(float), 0,
			zero_matrix, 0, NULL, NULL);

	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 10; ++j)
			printf("%6.1f", zero_matrix[i * 10 + j]);
		printf("\n");
	}

	clReleaseMemObject(matrix_buffer);
	clReleaseCommandQueue(queue);
	clReleaseDevice(device);
	clReleaseContext(context);

	return 0;
}