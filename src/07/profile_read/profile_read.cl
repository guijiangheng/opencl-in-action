__kernel void profile_read(__global char16* c, int n) {
    for (int i = 0; i < n; ++i)
        c[i] = (char16)(5);
}
