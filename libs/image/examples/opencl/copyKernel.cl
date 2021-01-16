__kernel void copy(
    __global const uint *input,
    __global uint *output
) {
    size_t globalId = get_global_id(0);
    output[globalId] = input[globalId];
}
