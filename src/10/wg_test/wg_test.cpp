#define __CL_ENABLE_EXCEPTIONS
#define __CRT_SECURE_NO_WARNINGS

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
		auto program = loadProgram(ctx, "wg_test.cl");
		program.build(devices);
		Kernel kernel(program, "blank");

		auto wg_size = kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(devices[0]);
		auto wg_multiple = kernel.getWorkGroupInfo<CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE>(devices[0]);
		auto local_usage = kernel.getWorkGroupInfo<CL_KERNEL_LOCAL_MEM_SIZE>(devices[0]);
		auto private_usage = kernel.getWorkGroupInfo<CL_KERNEL_PRIVATE_MEM_SIZE>(devices[0]);

		printf("maximum work-group size is: %zu and work group multiple is %zu.\n", wg_size, wg_multiple);
		printf("local memory useage: %zu, private memory usage: %zu", local_usage, private_usage);
	} catch (Error& e) {
		cout << e.what() << ": Error code " << e.err() << endl;
	}
	return 0;
}