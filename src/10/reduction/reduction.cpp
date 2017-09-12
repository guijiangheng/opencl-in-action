#define __CL_ENABLE_EXCEPTIONS
#define __CRT_SECURE_NO_WARNINGS

#define ARRAY_SIZE 65536
#define NUM_KERNELS 2

#include <fstream>
#include <iostream>
#include <CL/cl.hpp>

using namespace cl;
using namespace std;

Program loadProgram(Context ctx, const char* filename) {
	ifstream file(filename);
	string str{ istreambuf_iterator<char>(file),
		istreambuf_iterator<char>() };
	Program::Sources source(1, make_pair(str.c_str(), str.length() + 1));
	return Program(ctx, source);
}

int main() {
	try {
		Context ctx(CL_DEVICE_TYPE_GPU);
		auto devices = ctx.getInfo<CL_CONTEXT_DEVICES>();
		auto program = loadProgram(ctx, "reduction.cl");
		program.build(devices);
		CommandQueue queue(ctx, devices[0], CL_QUEUE_PROFILING_ENABLE);

		float data[ARRAY_SIZE];
		for (int i = 0; i < ARRAY_SIZE; ++i) data[i] = i;
		Buffer data_buffer(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(data), data);
		const char* kernel_names[] = { "reduction_scalar", "reduction_vector" };
		auto local_size = devices[0].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

		for (int i = 0; i < NUM_KERNELS; ++i) {
			auto num_groups = ARRAY_SIZE / local_size;
			if (i == 1) num_groups /= 4;
			auto sums = new float[num_groups];
			Buffer sums_buffer(ctx, CL_MEM_WRITE_ONLY, sizeof(float) * num_groups);

			Kernel kernel(program, kernel_names[i]);
			LocalSpaceArg arg = cl::__local(sizeof(float) * (i == 0 ? local_size : local_size * 4));
			kernel.setArg(0, data_buffer);
			kernel.setArg(1, arg);
			kernel.setArg(2, sums_buffer);

			Event profile_event;
			auto global_size = (i == 0 ? ARRAY_SIZE : ARRAY_SIZE / 4);
			queue.enqueueNDRangeKernel(kernel, NullRange,
				NDRange(global_size), NDRange(local_size), NULL, &profile_event);

			queue.finish();
			auto start = profile_event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
			auto end = profile_event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
			auto total_time = end - start;
			queue.enqueueReadBuffer(sums_buffer, CL_TRUE, 0, sizeof(float) * num_groups, sums);

			float sum = 0;
			for (int j = 0; j < num_groups; ++j)
				sum += sums[j];
			delete[] sums;

			printf("%s sum is: %f\n", kernel_names[i], sum);
			float actual_sum = ARRAY_SIZE / 2 * (ARRAY_SIZE - 1);
			if (fabs(sum - actual_sum) > 0.01 * fabs(sum))
				printf("Check failed.\n");
			else
				printf("Check passed.\n");
			printf("Total time: %lu\n\n", total_time);
		}
	} catch (Error& e) {
		cout << e.what() << ": Error code " << e.err() << endl;
	}

	return 0;
}