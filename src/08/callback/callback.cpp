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

void CL_CALLBACK checkData(cl_event, cl_int status, void* data) {
	auto buffer_data = (int*)data;
	bool check = true;
	for (int i = 0; i < 100; ++i) {
		if (buffer_data[i] != 2 * i) {
			check = false;
			break;
		}
	}
	if (check)
		cout << "The data is accurate." << endl;
	else
		cout << "The data is not accurate." << endl;
}

int main() {
	try {
		Context ctx(CL_DEVICE_TYPE_GPU);
		auto devices = ctx.getInfo<CL_CONTEXT_DEVICES>();
		auto program = loadProgram(ctx, "callback.cl");
		program.build(devices);
		Kernel kernel(program, "callback");

		int data[100];
		for (int i = 0; i < 100; ++i) data[i] = i;
		Buffer buffer(ctx, CL_MEM_READ_WRITE |
			CL_MEM_COPY_HOST_PTR, sizeof(data), data);
		kernel.setArg(0, buffer);

		CommandQueue queue(ctx, devices[0]);
		queue.enqueueTask(kernel);
		Event readEvent;
		queue.enqueueReadBuffer(buffer, CL_FALSE, 0, sizeof(data), data, NULL, &readEvent);
		readEvent.setCallback(CL_COMPLETE, &checkData, data);
	} catch (Error& e) {
		cout << e.what() << ": Error code " << e.err() << endl;
	}

	return 0;
}