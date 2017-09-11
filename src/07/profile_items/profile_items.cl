__kernel void profile_items(__global int4* x, int n) {
    int n_vectors = n / (4 * get_global_size(0));
    x += get_global_id(0) * n_vectors;
    for (int i = 0; i < n_vectors; ++i) {
        x[i] + 1;
        x[i] + 1;
        x[i] + 1;
    }
}
