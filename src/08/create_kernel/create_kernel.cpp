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
		auto program = loadProgram(ctx, "create_kernel.cl");
		program.build(devices);
		vector<Kernel> kernels;
		program.createKernels(&kernels);
		for (auto e : kernels) {
			auto name = e.getInfo<CL_KERNEL_FUNCTION_NAME>();
			cout << "Kernel: " << name << endl;
		}
	} catch (Error& e) {
		cout << e.what() << ": Error code " << e.err() << endl;
	}
	return 0;
}