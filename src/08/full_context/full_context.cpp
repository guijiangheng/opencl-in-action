#define _CRT_SECURE_NO_WARNINGS

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

int main() {
	try {
		auto platform = getAMDPlatform();
		Context context(CL_DEVICE_TYPE_GPU);
		auto devices = context.getInfo<CL_CONTEXT_DEVICES>();
		for (auto e : devices) {
			auto device_name = e.getInfo<CL_DEVICE_NAME>();
			cout << device_name.c_str() << endl;
		}
	} catch (exception& e) {
		cout << e.what() << endl;
	}

	return 0;
}