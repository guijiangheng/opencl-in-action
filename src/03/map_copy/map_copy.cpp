#include <stdio.h>
#include <string.h>

#include <CL/cl.h>

int main() {
	cl_platform_id platform;
	clGetPlatformIDs(1, &platform, NULL);
	cl_device_id device;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
	cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

	float data_one[100];
	float data_two[100];
	float result[100];
	for (int i = 0; i < 100; ++i) {
		data_one[i] =  i;
		data_two[i] = -i;
		result[i] = 0;
	}

	cl_mem buffer_one = clCreateBuffer(
		context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		sizeof(data_one), data_one, NULL);
	cl_mem buffer_two = clCreateBuffer(
		context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		sizeof(data_two), data_two, NULL);

	clEnqueueCopyBuffer(
		queue, buffer_one, buffer_two,
		0, 0, sizeof(data_one), 0, NULL, NULL);

	void* mapped_memory = clEnqueueMapBuffer(
		queue, buffer_two, CL_TRUE, CL_MAP_READ,
		0, sizeof(data_two), 0, NULL, NULL, NULL);

	memcpy(result, mapped_memory, sizeof(data_two));
	clEnqueueUnmapMemObject(queue, buffer_two, mapped_memory, 0, NULL, NULL);

	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j)
			printf("%6.1f", result[i * 10 + j]);
		printf("\n");
	}

	clReleaseMemObject(buffer_one);
	clReleaseMemObject(buffer_two);
	clReleaseCommandQueue(queue);
	clReleaseDevice(device);
	clReleaseContext(context);
	
	return 0;
}
