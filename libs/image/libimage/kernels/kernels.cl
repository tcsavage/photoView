__kernel void apply3DLut_F32_F32(
    __read_only image3d_t lutImage,
    sampler_t lutSampler,
    __global const float *inputImage,
    __global float *outputImage
) {
    size_t globalId = get_global_id(0);

    float3 colorIn = vload3(globalId, inputImage);
    float4 lutCoord = (float4)(colorIn, 0);
    float4 lutValue = read_imagef(lutImage, lutSampler, lutCoord);
    float3 colorOut = lutValue.xyz;
    vstore3(colorOut, globalId, outputImage);
}

__kernel void apply3DLut_masked_F32_F32(
    __read_only image3d_t lutImage,
    sampler_t lutSampler,
    __global const float *inputImage,
    __global const float *inputMask,
    __global float *outputImage
) {
    size_t globalId = get_global_id(0);

    float3 colorIn = vload3(globalId, inputImage);
    float4 lutCoord = (float4)(colorIn, 0);
    float4 lutValue = read_imagef(lutImage, lutSampler, lutCoord);
    float maskFactor = inputMask[globalId];
    float3 colorOut = (lutValue.xyz * maskFactor) + (colorIn * (1 - maskFactor));
    vstore3(colorOut, globalId, outputImage);
}

__kernel void finalize_F32_U8(
    __global const float *inputImage,
    __global uchar *outputImage
) {
    size_t globalId = get_global_id(0);

    float3 color = vload3(globalId, inputImage);
    vstore3(convert_uchar3(color * 256), globalId, outputImage);
}
