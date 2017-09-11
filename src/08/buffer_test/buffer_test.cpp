#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <iostream>
#include <CL/cl.hpp>

using namespace cl;
using namespace std;

Platform getAMDPlatform() {
	vector<Platform> platforms;
	Platform::get(&platforms);
	for (auto e : platforms) {
		auto platform_name = e.getInfo<CL_PLATFORM_NAME>();
		if (platform_name.find("AMD") != string::npos) {}
		return e;
	}
	throw exception("cannot find amd platform!");
}

Program buildProgram(Context context, const char* filename) {
	ifstream file(filename);
	string str{istreambuf_iterator<char>(file),
			   istreambuf_iterator<char>()};
	Program::Sources source(1, make_pair(str.c_str(), str.length() + 1));
	Program program(context, source);
	program.build();
	return program;
}

int main() {
	try {
		Platform platform = getAMDPlatform();
		Context context(CL_DEVICE_TYPE_GPU);
		auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
		auto program = buildProgram(context, "buffer_test.cl");
		Kernel kernel(program, "blank");

		float full_matrix[80], zero_matrix[80];
		for (int i = 0; i < 80; ++i) {
			full_matrix[i] = i;
			zero_matrix[i] = 0;
		}

		Buffer buffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
			sizeof(zero_matrix), zero_matrix);
		kernel.setArg(0, buffer);

		CommandQueue queue(context, devices[0]);
		queue.enqueueTask(kernel);
		queue.enqueueWriteBuffer(buffer, CL_TRUE, 0,
			sizeof(full_matrix), full_matrix);

		cl::size_t<3> buffer_origin;
		buffer_origin[0] = 5 * sizeof(float);
		buffer_origin[1] = 3;
		buffer_origin[2] = 0;

		cl::size_t<3> host_origin;
		host_origin[0] = sizeof(float);
		host_origin[1] = 1;
		host_origin[2] = 0;

		cl::size_t<3> region;
		region[0] = 4 * sizeof(float);
		region[1] = 4;
		region[2] = 1;

		queue.enqueueReadBufferRect(buffer, CL_TRUE,
			buffer_origin, host_origin, region,
			10 * sizeof(float), 0, 10 * sizeof(float), 0, zero_matrix);

		for (int i = 0; i < 8; ++i) {
			for (int j = 0; j < 8; ++j)
				printf("%6.1f", zero_matrix[i * 10 + j]);
			printf("\n");
		}
	} catch (exception& e) {
		cout << e.what() << endl;
	}

	return 0;
}