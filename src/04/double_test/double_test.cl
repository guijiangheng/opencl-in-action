#ifdef FP_64
#pragma OPENCL_EXTENSION cl_khr_fp64 : enable
#endif

__kernel void double_test(
        float a, float b,
        __global float* out) {
#ifdef FP_64
    double c = (double)(a / b);
    *out = c;
#else
    *out = a * b;
#endif
}
