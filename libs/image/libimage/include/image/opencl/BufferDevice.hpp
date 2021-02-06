#pragma once

#define CL_TARGET_OPENCL_VERSION 120
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <image/SmallVector.hpp>
#include <image/memory/Buffer.hpp>
#include <image/opencl/Context.hpp>
#include <image/opencl/Handle.hpp>

namespace image::memory {

    using MemObjectHandle = opencl::Handle<cl_mem, &clRetainMemObject, &clReleaseMemObject>;

    struct OpenCLDevice final : public AbstractDevice {
        opencl::ContextHandle ctx;
        opencl::CommandQueueHandle queue;

        void malloc(Buffer &buf) noexcept override;

        void free(Buffer &buf) noexcept override;

        void sync(Buffer &) noexcept override;

        void copyDeviceToHost(Buffer &buf) noexcept override;

        void copyHostToDevice(Buffer &buf) noexcept override;

        explicit OpenCLDevice(const opencl::ContextHandle &ctx, const opencl::CommandQueueHandle &queue) noexcept;
    };

    struct OpenCLImageDevice final : public AbstractDevice {
        opencl::ContextHandle ctx;
        opencl::CommandQueueHandle queue;
        SmallVector<Size, 3> imageSize;

        void malloc(Buffer &buf) noexcept override;

        void free(Buffer &buf) noexcept override;

        void sync(Buffer &) noexcept override;

        void copyDeviceToHost(Buffer &buf) noexcept override;

        void copyHostToDevice(Buffer &buf) noexcept override;

        explicit OpenCLImageDevice(
            const opencl::ContextHandle &ctx,
            const opencl::CommandQueueHandle &queue,
            const SmallVectorImpl<Size> &imageSize
        ) noexcept;
    };

}
