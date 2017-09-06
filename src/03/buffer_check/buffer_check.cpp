#include <stdio.h>
#include <stdlib.h>

#include <CL/cl.h>

int main() {
	cl_platform_id platform;
	clGetPlatformIDs(1, &platform, NULL);
	cl_device_id device;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);

	float data[100];
	cl_mem main_buffer = clCreateBuffer(
			context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
			sizeof(data), data, NULL);

	cl_buffer_region region = { 30 * sizeof(float), 20 * sizeof(float) };
	cl_mem sub_buffer = clCreateSubBuffer(
			main_buffer, CL_MEM_READ_ONLY,
			CL_BUFFER_CREATE_TYPE_REGION, &region, NULL);

	size_t main_buffer_size, sub_buffer_size;
	clGetMemObjectInfo(main_buffer, CL_MEM_SIZE, sizeof(main_buffer_size), &main_buffer_size, NULL);
	clGetMemObjectInfo(sub_buffer, CL_MEM_SIZE, sizeof(sub_buffer_size), &sub_buffer_size, NULL);
	printf("Main buffer size: %lu\n", main_buffer_size);
	printf("Sub buffer size: %lu\n", sub_buffer_size);

	//只有在创建Buffer时使用USE_HOST_PTR，才能得到HOST_PTR
	void* main_buffer_mem, *sub_buffer_mem;
	clGetMemObjectInfo(main_buffer, CL_MEM_HOST_PTR, sizeof(main_buffer_mem), &main_buffer_mem, NULL);
	clGetMemObjectInfo(sub_buffer, CL_MEM_HOST_PTR, sizeof(sub_buffer_mem), &sub_buffer_mem, NULL);
	printf("Main buffer memory address: %p\n", main_buffer_mem);
	printf("Sub buffer memory address: %p\n", sub_buffer_mem);

	clReleaseMemObject(main_buffer);
	clReleaseMemObject(sub_buffer);
	clReleaseDevice(device);
	clReleaseContext(context);

	return 0;
}
