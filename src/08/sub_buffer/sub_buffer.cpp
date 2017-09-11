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
		auto program = loadProgram(ctx, "sub_buffer.cl");
		program.build(devices);
		Kernel kernel(program, "sub_buffer");

		float data[200];
		Buffer main_buffer(ctx, CL_MEM_READ_ONLY |
			CL_MEM_USE_HOST_PTR, sizeof(data), data);
		kernel.setArg(0, main_buffer);

		::size_t config[2] = { 70 * sizeof(float), 20 * sizeof(float) };
		auto sub_buffer = main_buffer.createSubBuffer(
			CL_MEM_READ_ONLY, CL_BUFFER_CREATE_TYPE_REGION, config);
		kernel.setArg(1, sub_buffer);

		cout << "Main buffer size: " << main_buffer.getInfo<CL_MEM_SIZE>() << endl;
		cout << "Main buffer memory location: " << main_buffer.getInfo<CL_MEM_HOST_PTR>() << endl;
		cout << "Sub-buffer size: " << sub_buffer.getInfo<CL_MEM_SIZE>() << endl;
		cout << "Sub-buffer memory location: " << sub_buffer.getInfo<CL_MEM_HOST_PTR>() << endl;
	} catch (Error& e) {
		cout << e.what() << ": Error code " << e.err() << endl;
	}
	return 0;
}