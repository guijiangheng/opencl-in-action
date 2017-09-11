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

void CL_CALLBACK printMessage(cl_event e, cl_int status, void* data) {
	cout << "The kernel has executed." << endl;
}

int main() {
	try {
		Context ctx(CL_DEVICE_TYPE_GPU);
		auto devices = ctx.getInfo<CL_CONTEXT_DEVICES>();
		auto program = loadProgram(ctx, "user_event.cl");
		program.build(devices);
		Kernel kernel(program, "user_event");

		int data[10];
		Buffer buffer(ctx, CL_MEM_READ_WRITE |
			CL_MEM_COPY_HOST_PTR, sizeof(data), data);
		kernel.setArg(0, buffer);

		Event callbackEvent;
		UserEvent userEvent(ctx);
		vector<Event> waitList;
		waitList.push_back((Event)userEvent);
		CommandQueue queue(ctx, devices[0]);
		queue.enqueueTask(kernel, &waitList, &callbackEvent);
		callbackEvent.setCallback(CL_COMPLETE, &printMessage);

		cout << "Press enter to execute kernel." << endl;
		getchar();
		userEvent.setStatus(CL_COMPLETE);
	} catch (Error& e) {
		cout << e.what() << ": Error code " << e.err() << endl;
	}

	return 0;
}