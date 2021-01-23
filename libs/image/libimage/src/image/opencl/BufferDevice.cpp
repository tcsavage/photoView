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
        // std::cerr << "[OpenCLDevice] copying: " << std::hex << buf.size << std::dec
        //           << " bytes to host ptr " << std::hex << buf.hostPtr << std::dec << "\n";
        clEnqueueReadBuffer(queue.get(), handle, true, 0, buf.size, buf.hostPtr, 0, nullptr, nullptr);
    }

    void OpenCLDevice::copyHostToDevice(Buffer &buf) noexcept {
        auto handle = reinterpret_cast<cl_mem>(buf.deviceHandle);
        // std::cerr << "[OpenCLDevice] copying: " << std::hex << buf.size << std::dec
        //           << " bytes from host ptr " << std::hex << buf.hostPtr << std::dec << "\n";
        clEnqueueWriteBuffer(queue.get(), handle, true, 0, buf.size, buf.hostPtr, 0, nullptr, nullptr);
    }

    OpenCLDevice::OpenCLDevice(const opencl::ContextHandle &ctx, const opencl::CommandQueueHandle &queue) noexcept : ctx(ctx), queue(queue) {
        ctx.incRef();
        queue.incRef();
    }



    void OpenCLImageDevice::malloc(Buffer &buf) noexcept {
        std::cerr << "[OpenCLImageDevice] Creating image of size " << std::hex << imageSize << std::dec << "\n";
        cl_int ret;
        // TODO: Generalise format and desc
        cl_image_format format;
        format.image_channel_order = CL_RGBA;
        format.image_channel_data_type = CL_FLOAT;
        cl_image_desc desc;
        desc.image_type = CL_MEM_OBJECT_IMAGE3D;
        desc.image_width = imageSize.at(0);
        desc.image_height = imageSize.at(1);
        desc.image_depth = imageSize.at(2);
        desc.image_array_size = 0;
        desc.image_row_pitch = 0;
        desc.image_slice_pitch = 0;
        desc.num_mip_levels = 0;
        desc.num_samples = 0;
        desc.buffer = nullptr;
        cl_mem handle = clCreateImage(ctx.get(), CL_MEM_READ_WRITE, &format, &desc, nullptr, &ret);
        buf.deviceHandle = reinterpret_cast<intptr_t>(handle);
    }

    void OpenCLImageDevice::free(Buffer &buf) noexcept {
        auto handle = reinterpret_cast<cl_mem>(buf.deviceHandle);
        clReleaseMemObject(handle);
    }

    void OpenCLImageDevice::sync(Buffer &) noexcept {
        clFinish(queue.get());
    }

    void OpenCLImageDevice::copyDeviceToHost(Buffer &buf) noexcept {
        cl_event ev;
        auto handle = reinterpret_cast<cl_mem>(buf.deviceHandle);
        // std::cerr << "[OpenCLImageDevice] copying: " << std::hex << buf.size << std::dec
        //           << " bytes to host ptr " << std::hex << buf.hostPtr << std::dec << "\n";
        std::array<std::size_t, 3> origin { 0, 0, 0 };
        std::array<std::size_t, 3> region { imageSize.at(0), imageSize.at(1), imageSize.at(2) };
        auto ret = clEnqueueReadImage(queue.get(), handle, true, origin.data(), region.data(), 0, 0, buf.hostPtr, 0, nullptr, nullptr);
        if (ret != CL_SUCCESS) {
            std::cerr << "[OpenCLImageDevice] error copying from device to host: " << opencl::Error(ret) << "\n";
        }
        ret = clWaitForEvents(1, &ev);
        if (ret != CL_SUCCESS) {
            std::cerr << "[OpenCLImageDevice] error copying from device to host: " << opencl::Error(ret) << "\n";
        }
    }

    void OpenCLImageDevice::copyHostToDevice(Buffer &buf) noexcept {
        cl_event ev;
        auto handle = reinterpret_cast<cl_mem>(buf.deviceHandle);
        // std::cerr << "[OpenCLImageDevice] copying: " << std::hex << buf.size << std::dec
        //           << " bytes from host ptr " << std::hex << buf.hostPtr << std::dec << "\n";
        std::array<std::size_t, 3> origin { 0, 0, 0 };
        std::array<std::size_t, 3> region { imageSize.at(0), imageSize.at(1), imageSize.at(2) };
        auto ret = clEnqueueWriteImage(queue.get(), handle, true, origin.data(), region.data(), 0, 0, buf.hostPtr, 0, nullptr, &ev);
        if (ret != CL_SUCCESS) {
            std::cerr << "[OpenCLImageDevice] error copying from host to device: " << opencl::Error(ret) << "\n";
        }
        ret = clWaitForEvents(1, &ev);
        if (ret != CL_SUCCESS) {
            std::cerr << "[OpenCLImageDevice] error copying from host to device: " << opencl::Error(ret) << "\n";
        }
    }

    OpenCLImageDevice::OpenCLImageDevice(
        const opencl::ContextHandle &ctx,
        const opencl::CommandQueueHandle &queue,
        const Shape &imageSize
    ) noexcept : ctx(ctx), queue(queue), imageSize(imageSize) {
        ctx.incRef();
        queue.incRef();
    }

}
