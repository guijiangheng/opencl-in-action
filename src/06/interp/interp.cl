constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                             CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void interp(read_only image2d_t src_img,
                     write_only image2d_t dst_img) {
    float2 input_coord = (float2)(
            get_global_id(0) + (1.0f / (SCALE * 2)),
            get_global_id(1) + (1.0f / (SCALE * 2)));
    int2 output_coord = (int2)(
            SCALE * get_global_id(0),
            SCALE * get_global_id(1));
    for(int i = 0; i < SCALE; ++i)
        for(int j = 0; j < SCALE; ++j) {
            float4 pixel = read_imagef(
                src_img, sampler,
                input_coord + (float2)(1.0f * i / SCALE, 1.0f * j / SCALE));
            write_imagef(dst_img, output_coord + (int2)(i, j), pixel);
        }
}
