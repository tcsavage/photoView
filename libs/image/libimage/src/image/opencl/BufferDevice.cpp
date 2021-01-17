#include <image/opencl/BufferDevice.hpp>

#include <iostream>

namespace image::memory {

    void OpenCLDevice::malloc(Buffer &buf) noexcept {
        std::cerr << "[OpenCLDevice] Creating buffer of size " << std::hex << buf.size << std::dec << "\n";
        cl_int ret;
        cl_mem handle = clCreateBuffer(ctx.get(), CL_MEM_READ_WRITE, buf.size, nullptr, &ret);
        buf.deviceHandle = reinterpret_cast<intptr_t>(handle);
    }

    void OpenCLDevice::free(Buffer &buf) noexcept {
        auto handle = reinterpret_cast<cl_mem>(buf.deviceHandle);
        clReleaseMemObject(handle);
    }

    void OpenCLDevice::sync(Buffer &) noexcept {
        clFinish(queue.get());
    }

    void OpenCLDevice::copyDeviceToHost(Buffer &buf) noexcept {
        auto handle = reinterpret_cast<cl_mem>(buf.deviceHandle);
        std::cerr << "[OpenCLDevice] copying: " << std::hex << buf.size << std::dec
                  << " bytes to host ptr " << std::hex << buf.hostPtr << std::dec << "\n";
        clEnqueueReadBuffer(queue.get(), handle, true, 0, buf.size, buf.hostPtr, 0, nullptr, nullptr);
    }

    void OpenCLDevice::copyHostToDevice(Buffer &buf) noexcept {
        auto handle = reinterpret_cast<cl_mem>(buf.deviceHandle);
        std::cerr << "[OpenCLDevice] copying: " << std::hex << buf.size << std::dec
                  << " bytes from host ptr " << std::hex << buf.hostPtr << std::dec << "\n";
        clEnqueueWriteBuffer(queue.get(), handle, true, 0, buf.size, buf.hostPtr, 0, nullptr, nullptr);
    }

    OpenCLDevice::OpenCLDevice(const opencl::ContextHandle &ctx, const opencl::CommandQueueHandle &queue) noexcept : ctx(ctx), queue(queue) {
        ctx.incRef();
        queue.incRef();
    }

}
