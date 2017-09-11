__kernel void callback(__global int4* x) {
    for(int i = 0; i < 25; ++i)
        x[i] *= 2;
}
