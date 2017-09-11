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
		auto program = loadProgram(ctx, "profile.cl");
		program.build(devices);
		Kernel kernel(program, "profile");

		int data[10];
		Buffer buffer(ctx, CL_MEM_READ_WRITE |
			CL_MEM_COPY_HOST_PTR, sizeof(data), data);
		kernel.setArg(0, buffer);

		Event profileEvent;
		CommandQueue queue(ctx, devices[0], CL_QUEUE_PROFILING_ENABLE);
		queue.enqueueTask(kernel, NULL, &profileEvent);
		queue.finish();

		auto start = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_START>();
		auto end = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_END>();
		cout << "Elapsed time: " << end - start << " ns." << endl;
	} catch (Error& e) {
		cout << e.what() << ": Error code " << e.err() << endl;
	}

	return 0;
}