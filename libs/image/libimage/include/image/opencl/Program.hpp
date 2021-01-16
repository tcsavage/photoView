#pragma once

#include <cassert>
#include <concepts>
#include <map>

#include <image/CoreTypes.hpp>
#include <image/Expected.hpp>
#include <image/NDArray.hpp>
#include <image/opencl/BufferDevice.hpp>
#include <image/opencl/Context.hpp>
#include <image/opencl/Handle.hpp>

namespace image::opencl {

    using ProgramHandle = Handle<cl_program, &clRetainProgram, &clReleaseProgram>;
    using KernelHandle = Handle<cl_kernel, &clRetainKernel, &clReleaseKernel>;

    struct Kernel {
        KernelHandle handle;

        cl_uint getNumArgs() const noexcept;

        template <class T>
        requires std::integral<T> || std::floating_point<T>
        Expected<void, Error> setArg(cl_uint idx, const T& value) noexcept {
            cl_int ret = clSetKernelArg(handle.get(), idx, sizeof(T), &value);
            if (ret != CL_SUCCESS) {
                return Unexpected(Error(ret));
            }
            return success;
        }

        Expected<void, Error> setArg(cl_uint idx, const std::nullptr_t&) noexcept;
        Expected<void, Error> setArg(cl_uint idx, const cl_mem& mem) noexcept;
        Expected<void, Error> setArg(cl_uint idx, const memory::Buffer& buf) noexcept;
        Expected<void, Error> setArg(cl_uint idx, const NDArrayBase& arr) noexcept;

        template <class T, class... Ts>
        Expected<void, Error> setArgsFromIdx(cl_uint idx, T &arg, Ts&&... args) noexcept {
            auto res = setArg(idx, arg);
            if (res.hasError()) {
                return res;
            }
            if constexpr(sizeof...(Ts) > 0) {
                return setArgsFromIdx<Ts...>(++idx, std::forward<Ts>(args)...);
            } else {
                return success;
            }
        }

        template <class... Ts>
        Expected<void, Error> setArgs(Ts&&... args) noexcept {
            assert(sizeof...(Ts) == getNumArgs());
            return setArgsFromIdx(0, std::forward<Ts>(args)...);
        }

        Expected<void, Error> run(const CommandQueueHandle &queue, const Shape &globalWorkShape) noexcept;
    };

    struct Program {
        ProgramHandle handle;

        static Expected<Program, Error> fromSource(const Context &ctx, const String &src) noexcept;

        Expected<void, Error> build() noexcept;

        Expected<Kernel, Error> getKernel(const String &name) noexcept;
    };

}
