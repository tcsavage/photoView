#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#define CL_TARGET_OPENCL_VERSION 120
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include <image/opencl/Handle.hpp>
#include <image/opencl/Types.hpp>

namespace image::opencl {

    struct Error : public std::exception {
        enum struct Code : cl_int {
            UNKNOWN_ERROR = 0,
            BUILD_PROGRAM_FAILURE = CL_BUILD_PROGRAM_FAILURE,
            COMPILER_NOT_AVAILABLE = CL_COMPILER_NOT_AVAILABLE,
            INVALID_BINARY = CL_INVALID_BINARY,
            INVALID_BUILD_OPTIONS = CL_INVALID_BUILD_OPTIONS,
            INVALID_COMMAND_QUEUE = CL_INVALID_COMMAND_QUEUE,
            INVALID_CONTEXT = CL_INVALID_CONTEXT,
            INVALID_DEVICE = CL_INVALID_DEVICE,
            INVALID_EVENT_WAIT_LIST = CL_INVALID_EVENT_WAIT_LIST,
            INVALID_GLOBAL_OFFSET = CL_INVALID_GLOBAL_OFFSET,
            INVALID_GLOBAL_WORK_SIZE = CL_INVALID_GLOBAL_WORK_SIZE,
            INVALID_IMAGE_SIZE = CL_INVALID_IMAGE_SIZE,
            INVALID_KERNEL_ARGS = CL_INVALID_KERNEL_ARGS,
            INVALID_ARG_INDEX = CL_INVALID_ARG_INDEX,
            INVALID_ARG_VALUE = CL_INVALID_ARG_VALUE,
            INVALID_ARG_SIZE = CL_INVALID_ARG_SIZE,
            INVALID_KERNEL = CL_INVALID_KERNEL,
            INVALID_OPERATION = CL_INVALID_OPERATION,
            INVALID_PROGRAM = CL_INVALID_PROGRAM,
            INVALID_PROGRAM_EXECUTABLE = CL_INVALID_PROGRAM_EXECUTABLE,
            INVALID_VALUE = CL_INVALID_VALUE,
            INVALID_WORK_DIMENSION = CL_INVALID_WORK_DIMENSION,
            INVALID_WORK_GROUP_SIZE = CL_INVALID_WORK_GROUP_SIZE,
            INVALID_WORK_ITEM_SIZE = CL_INVALID_WORK_ITEM_SIZE,
            INVALID_PLATFORM = CL_INVALID_PLATFORM,
            MEM_OBJECT_ALLOCATION_FAILURE = CL_MEM_OBJECT_ALLOCATION_FAILURE,
            MISALIGNED_SUB_BUFFER_OFFSET = CL_MISALIGNED_SUB_BUFFER_OFFSET,
            OUT_OF_HOST_MEMORY = CL_OUT_OF_HOST_MEMORY,
            OUT_OF_RESOURCES = CL_OUT_OF_RESOURCES,
            NV_ILLEGAL_READ_OR_WRITE_TO_BUFFER = -9999,
        };

        Error() { generateMsg(); }
        explicit Error(Code code) : code(code) { generateMsg(); }
        explicit Error(cl_int code) : code(static_cast<Code>(code)) { generateMsg(); }

        void generateMsg() noexcept;
        
        virtual const char *what() const noexcept override;

        Code code { Code::UNKNOWN_ERROR };
        std::string msg;
    };

    std::ostream& operator<<(std::ostream& out, const Error &error);

    struct Device {
        enum struct Type : cl_device_type {
            CPU = CL_DEVICE_TYPE_CPU,
            GPU = CL_DEVICE_TYPE_GPU,
            ACCELERATOR = CL_DEVICE_TYPE_ACCELERATOR,
            CUSTOM = CL_DEVICE_TYPE_CUSTOM,
            DEFAULT = CL_DEVICE_TYPE_DEFAULT,
            ALL = CL_DEVICE_TYPE_ALL,
        };

        cl_device_id id;
        std::string name;
        std::string vendor;
        Type type;
        size_t maxImageWidth;
        size_t maxImageHeight;
        bool imageSupport;
        cl_uint maxComputeUnits;
        cl_uint maxWorkItemDims;
    };

    struct Platform {
        cl_platform_id id;
        std::string name;
        std::string vendor;
        std::vector<Device> getDevices();
    };

    struct Configurator {
        std::vector<Platform> getPlatforms();
        void dumpPlatforms();
        Device selectDevice();
    };

    class Context {
    public:
        const ContextHandle &getHandle() const;

        Device &getDevice();
        cl_device_id getDeviceId() const;

        Context();
        explicit Context(cl_context handle);
        explicit Context(const Device &device);
        
        Context(Context &&other) noexcept;
        Context &operator=(Context &&other) noexcept;

        Context(const Context &other) = delete;
        Context &operator=(const Context &other) = delete;

    private:
        ContextHandle handle;
        Device device;
    };

    class CommandQueue {
    public:
        const CommandQueueHandle &getHandle() const;

        CommandQueue();
        CommandQueue(const Context &context);
        ~CommandQueue();
        
        CommandQueue(CommandQueue &&other) noexcept;
        CommandQueue &operator=(CommandQueue &&other) noexcept;
        
        CommandQueue(const CommandQueue &other) = delete;
        CommandQueue &operator=(const CommandQueue &other) = delete;

    private:
        CommandQueueHandle handle;
    };

}
