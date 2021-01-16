#include <image/opencl/Program.hpp>

#include <array>
#include <vector>

namespace image::opencl {

    cl_uint Kernel::getNumArgs() const noexcept {
        cl_uint out = 0;
        clGetKernelInfo(handle.get(), CL_KERNEL_NUM_ARGS, sizeof(cl_uint), &out, nullptr);
        return out;
    }

    Expected<void, Error> Kernel::setArg(cl_uint idx, const std::nullptr_t&) noexcept {
        cl_int ret = clSetKernelArg(handle.get(), idx, 0, nullptr);
        if (ret != CL_SUCCESS) {
            return Unexpected(Error(ret));
        }
        return success;
    }

    Expected<void, Error> Kernel::setArg(cl_uint idx, const cl_mem& mem) noexcept {
        cl_int ret = clSetKernelArg(handle.get(), idx, sizeof(cl_mem), &mem);
        if (ret != CL_SUCCESS) {
            return Unexpected(Error(ret));
        }
        return success;
    }

    Expected<void, Error> Kernel::setArg(cl_uint idx, const memory::Buffer& buf) noexcept {
        auto dev = std::dynamic_pointer_cast<memory::OpenCLDevice>(buf.device);
        if (!dev) {
            return Unexpected(Error(Error::Code::INVALID_VALUE));
        }
        auto memHandle = reinterpret_cast<cl_mem>(buf.deviceHandle);
        return setArg(idx, memHandle);
    }

    Expected<void, Error> Kernel::setArg(cl_uint idx, const NDArrayBase& arr) noexcept {
        return setArg(idx, arr.buffer());
    }

    Expected<void, Error> Kernel::run(const CommandQueueHandle &queue, const Shape &globalWorkShape) noexcept {
        cl_int ret = clEnqueueNDRangeKernel(
            queue.get(), handle.get(),
            globalWorkShape.dims().size(), nullptr, globalWorkShape.begin(),
            nullptr, 0, nullptr, nullptr
        );
        if (ret != CL_SUCCESS) {
            return Unexpected(Error(ret));
        }
        return success;
    }

    Expected<Program, Error> Program::fromSource(const Context &ctx, const String &src) noexcept {
        cl_int ret;
        std::array<const char*, 1> srcs { src.c_str() };
        std::array<std::size_t, 1> srcLens { src.size() };
        auto handle = clCreateProgramWithSource(ctx.getHandle().get(), srcs.size(), srcs.data(), srcLens.data(), &ret);
        if (handle == 0 || ret != CL_SUCCESS) {
            return Unexpected(Error(ret));
        }
        Program prog { ProgramHandle::takeOwnership(handle) };
        return prog;
    }

    Expected<void, Error> Program::build() noexcept {
        cl_int ret;

        // Get context.
        cl_context ctx;
        ret = clGetProgramInfo(handle.get(), CL_PROGRAM_CONTEXT, sizeof(cl_context), &ctx, nullptr);
        if (ret != CL_SUCCESS) { return Unexpected(Error(ret)); }
        // Get devices.
        cl_uint numDevices { 0 };
        ret = clGetContextInfo(ctx, CL_CONTEXT_NUM_DEVICES, sizeof(cl_uint), &numDevices, nullptr);
        if (ret != CL_SUCCESS) { return Unexpected(Error(ret)); }
        std::vector<cl_device_id> devices;
        devices.reserve(numDevices);
        ret = clGetContextInfo(ctx, CL_CONTEXT_DEVICES, sizeof(cl_device_id) * numDevices, devices.data(), nullptr);
        if (ret != CL_SUCCESS) { return Unexpected(Error(ret)); }

        // Build.
        ret = clBuildProgram(handle.get(), numDevices, devices.data(), nullptr, nullptr, nullptr);
        // if (ret == CL_BUILD_PROGRAM_FAILURE) {
        //     size_t logSize;
        //     clGetProgramBuildInfo(handle, device.id, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
        //     char *log = new char[logSize];
        //     clGetProgramBuildInfo(handle, device.id, CL_PROGRAM_BUILD_LOG, logSize, log, nullptr);
        //     std::cerr << "Error building program:\n" << log << "End of build log.\n";
        //     delete[] log;
        //     throw Error(ret);
        // }
        if (ret != CL_SUCCESS) {
            return Unexpected(Error(ret));
        }

        return success;
    }

    Expected<Kernel, Error> Program::getKernel(const String &name) noexcept {
        cl_int ret;
        auto kernelHandle = clCreateKernel(handle.get(), name.c_str(), &ret);
        if (ret != CL_SUCCESS) {
            return Unexpected(Error(ret));
        }
        Kernel kernel { KernelHandle::takeOwnership(kernelHandle) };
        return kernel;
    }

}
