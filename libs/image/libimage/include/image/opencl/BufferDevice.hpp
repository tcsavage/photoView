#pragma once

#define CL_TARGET_OPENCL_VERSION 120
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <image/memory/Buffer.hpp>
#include <image/opencl/Context.hpp>
#include <image/opencl/Handle.hpp>

namespace image::memory {

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

}
