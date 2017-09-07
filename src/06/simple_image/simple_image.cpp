#define _CRT_SECURE_NO_WARNINGS
#define PNG_DEBUG 3

#include <png.h>
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

void read_image(const char* filename, png_bytep* data, size_t* w, size_t* h) {
	FILE* file = fopen(filename, "rb");
	auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	auto info_ptr = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, file);
	png_read_info(png_ptr, info_ptr);
	*w = png_get_image_width(png_ptr, info_ptr);
	*h = png_get_image_height(png_ptr, info_ptr);
	*data = (png_bytep)malloc(*h * png_get_rowbytes(png_ptr, info_ptr));
	for (int i = 0; i < *h; ++i)
		png_read_row(png_ptr, *data + i * png_get_rowbytes(png_ptr, info_ptr), NULL);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(file);
}

void write_image(const char* filename, png_bytep data, size_t w, size_t h) {
	FILE* output = fopen(filename, "wb");
	auto png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	auto info_ptr = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, output);
	png_set_IHDR(png_ptr, info_ptr, w, h, 16,
				 PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);
	for (int i = 0; i < h; ++i)
		png_write_row(png_ptr, data + i * png_get_rowbytes(png_ptr, info_ptr));
	png_write_end(png_ptr, NULL);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(output);
}

int main() {
	auto platform = getAMDPlatform();
	cl_device_id device;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	auto context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
	auto queue = clCreateCommandQueue(context, device, 0, NULL);
	auto program = buildProgram(context, device, "simple_image.cl");
	auto kernel = clCreateKernel(program, "simple_image", NULL);

	png_bytep pixels;
	size_t width, height;
	read_image("blank.png", &pixels, &width, &height);

	cl_image_format format;
	format.image_channel_order = CL_LUMINANCE;
	format.image_channel_data_type = CL_UNORM_INT16;
	auto src_img = clCreateImage2D(
			context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
			&format, width, height, 0, pixels, NULL);
	auto dst_img = clCreateImage2D(
			context, CL_MEM_READ_ONLY,
			&format, width, height, 0, NULL, NULL);

	size_t global_size[] = { width, height };
	clSetKernelArg(kernel, 0, sizeof(cl_mem), &src_img);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &dst_img);
	clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_size, NULL, 0, NULL, NULL);

	size_t origin[] = { 0, 0, 0 };
	size_t region[] = { width, height, 1 };
	clEnqueueReadImage(queue, dst_img, CL_TRUE, origin, region, 0, 0, pixels, 0, NULL, NULL);
	write_image("output.png", pixels, width, height);
	free(pixels);

	clReleaseMemObject(src_img);
	clReleaseMemObject(dst_img);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseDevice(device);
	clReleaseContext(context);

	return 0;
}