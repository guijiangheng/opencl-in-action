__kernel void callback(__global float* buffer) {
    float4 five_vec = (float4)(5.0f);
    for (int i = 0; i < 1024; ++i)
        vstore4(five_vec, i, buffer);
}
