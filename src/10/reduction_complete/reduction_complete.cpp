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
	Context ctx(CL_DEVICE_TYPE_GPU);
	auto devices = ctx.getInfo<CL_CONTEXT_DEVICES>();
	auto program = loadProgram(ctx, "reduction_complete.cl");

	try {
		auto err = program.build(devices);
		CommandQueue queue(ctx, devices[0], CL_QUEUE_PROFILING_ENABLE);

		auto global_size = ARRAY_SIZE / 4;
		auto group_size = devices[0].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

		float data[ARRAY_SIZE];
		for (int i = 0; i < ARRAY_SIZE; ++i) data[i] = i;
		Buffer data_buffer(ctx, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(data), data);

		Event start_event;
		Kernel vector_kernel(program, "reduction_vector");
		vector_kernel.setArg(0, data_buffer);
		vector_kernel.setArg(1, cl::__local(sizeof(float) * 4 * group_size));
		queue.enqueueNDRangeKernel(vector_kernel,
			NullRange, NDRange(global_size), NDRange(group_size), NULL, &start_event);

		global_size /= group_size;
		while (global_size > group_size) {
			queue.enqueueNDRangeKernel(vector_kernel,
				NullRange, NDRange(global_size), NDRange(group_size));
			global_size /= group_size;
		}

		float sum;
		Buffer sum_buffer(ctx, CL_MEM_WRITE_ONLY, sizeof(sum));
		Kernel complete_kernel(program, "reduction_complete");
		complete_kernel.setArg(0, data_buffer);
		complete_kernel.setArg(1, cl::__local(sizeof(float) * 4 * global_size));
		complete_kernel.setArg(2, sum_buffer);

		Event end_event;
		queue.enqueueNDRangeKernel(complete_kernel,
			NullRange, NDRange(group_size), NullRange, NULL, &end_event);

		queue.finish();
		auto start = start_event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
		auto end = end_event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
		auto total_time = end - start;
		queue.enqueueReadBuffer(sum_buffer, CL_TRUE, 0, sizeof(float), &sum);

		float actual_sum = ARRAY_SIZE / 2 * (ARRAY_SIZE - 1);
		if (fabs(actual_sum - sum) > 0.01f * sum)
			printf("Check failed\n");
		else
			printf("Check passed\n");
		printf("Total time = %lu\n", total_time);
	} catch (Error& e) {
		cout << e.what() << ": Error code " << e.err() << endl;
		if (e.err() == CL_BUILD_PROGRAM_FAILURE) {
			for (auto device : devices) {
				auto status = program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device);
				if (status != CL_BUILD_ERROR)
					continue;
				auto log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
				cerr << "Build log: " << endl
					 << log << endl;
			}
		}
	}

	return 0;
}