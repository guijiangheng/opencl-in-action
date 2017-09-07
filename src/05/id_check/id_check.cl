__kernel void id_check(__global float* output) {
    size_t offset_0 = get_global_offset(0);
    size_t offset_1 = get_global_offset(1);
    size_t global_id_0 = get_global_id(0);
    size_t global_id_1 = get_global_id(1);
    size_t local_id_0 = get_local_id(0);
    size_t local_id_1 = get_local_id(1);
    size_t global_size_0 = get_global_size(0);

    int idx_0 = global_id_0 - offset_0;
    int idx_1 = global_id_1 - offset_1;
    int idx = global_size_0 * idx_1 + idx_0;

    float f = global_id_0 * 10.0f + global_id_1 * 1.0f;
    f += local_id_0 * 0.1f + local_id_1 * 0.01f;
    
    output[idx] = f;
}
