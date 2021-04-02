#pragma once

#define CL_TARGET_OPENCL_VERSION 120
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <image/opencl/Handle.hpp>

namespace image::opencl {

    using CommandQueueHandle = Handle<cl_command_queue, &clRetainCommandQueue, &clReleaseCommandQueue>;
    using ContextHandle = Handle<cl_context, &clRetainContext, &clReleaseContext>;
    using KernelHandle = Handle<cl_kernel, &clRetainKernel, &clReleaseKernel>;
    using MemObjectHandle = Handle<cl_mem, &clRetainMemObject, &clReleaseMemObject>;
    using ProgramHandle = Handle<cl_program, &clRetainProgram, &clReleaseProgram>;
    using SamplerHandle = Handle<cl_sampler, &clRetainSampler, &clReleaseSampler>;

    class CommandQueue;
    class Context;
    struct Device;
    struct Kernel;
    struct Program;

    struct Error;
    struct SetArgsError;
    
}

namespace image::memory {

    struct OpenCLDevice;
    struct OpenCLImageDevice;

}
