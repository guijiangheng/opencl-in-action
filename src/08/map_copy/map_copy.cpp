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
		auto program = loadProgram(ctx, "map_copy.cl");
		program.build(devices);
		Kernel kernel(program, "map_copy");

		float a[100], b[100], c[100];
		for (int i = 0; i < 100; ++i) {
			a[i] = i;
			b[i] = -i;
			c[i] = 0;
		}

		Buffer bufferA(ctx, CL_MEM_READ_WRITE |
			CL_MEM_COPY_HOST_PTR, sizeof(a), a);
		Buffer bufferB(ctx, CL_MEM_READ_WRITE |
			CL_MEM_COPY_HOST_PTR, sizeof(b), b);
		kernel.setArg(0, bufferA);
		kernel.setArg(1, bufferB);

		CommandQueue queue(ctx, devices[0]);
		queue.enqueueTask(kernel);
		queue.enqueueCopyBuffer(bufferA, bufferB, 0, 0, sizeof(a));
		auto mapped_memory = queue.enqueueMapBuffer(bufferB,
			CL_TRUE, CL_MAP_READ, 0, sizeof(a));
		memcpy(c, mapped_memory, sizeof(a));
		queue.enqueueUnmapMemObject(bufferB, mapped_memory);

		for (int i = 0; i < 10; ++i) {
			for (int j = 0; j < 10; ++j)
				printf("%6.1f", c[i * 10 + j]);
			printf("\n");
		}
	} catch (Error& e) {
		cout << e.what() << ": Error code " << e.err() << endl;
	}
	return 0;
}