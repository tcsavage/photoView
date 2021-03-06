__kernel void generate_overlay_image_F32_U8(
    __global const float *inputMask,
    __global uchar *outputImage
) {
    size_t globalId = get_global_id(0);

    float maskFactor = inputMask[globalId];
    float4 maskColor = (float4)(1.0f, 0.0f, 0.0f, maskFactor);
    vstore4(convert_uchar4(maskColor * 256), globalId, outputImage);
}
