__constant sampler_t sampler =
        CLK_NORMALIZED_COORDS_FALSE |
        CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void simple_image(read_only image2d_t src_img,
                           write_only image2d_t dst_img) {
   int2 coord = (int2)(get_global_id(0), get_global_id(1));
   uint4 color = read_imageui(src_img, sampler, coord);
   color.x -= coord.y * 0x4000 + coord.x * 0x1000;
   write_imageui(dst_img, coord, color);
}
