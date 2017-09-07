#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CL/cl.h>

// 我的电脑只有AMD能够工作
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

	cl_device_fp_config flag;
	clGetDeviceInfo(device, CL_DEVICE_SINGLE_FP_CONFIG,
					sizeof(flag), &flag, NULL);

	printf("Float processing features:\n");
	if (flag & CL_FP_INF_NAN)
		printf("INF and NaN values supported.\n");
	if (flag & CL_FP_DENORM)
		printf("Denormalized numbers supported.\n");
	if (flag & CL_FP_ROUND_TO_NEAREST)
		printf("Round to nearst even mode supported.\n");
	if (flag & CL_FP_ROUND_TO_INF)
		printf("Round to infinity mode supported.\n");
	if (flag & CL_FP_ROUND_TO_ZERO)
		printf("Round to zero mode supported.\n");
	if (flag & CL_FP_SOFT_FLOAT)
		printf("Floating-point multiply-and-add operation supported.\n");

	return 0;
}