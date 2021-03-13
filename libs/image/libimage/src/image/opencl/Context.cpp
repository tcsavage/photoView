#include <image/opencl/Context.hpp>

#include <iostream>
#include <sstream>

namespace image::opencl {

    namespace {
        void clNotify(const char *errinfo, const void *private_info, size_t cb, void *user_data) {
            (void)(private_info);
            (void)(cb);
            (void)(user_data);
            std::cout << "CL Notify! - " << errinfo << "\n";
        }
    }

    std::ostream& operator<<(std::ostream& out, const Error::Code value) {
        const char* s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
        switch(value){
            PROCESS_VAL(Error::Code::UNKNOWN_ERROR);
            PROCESS_VAL(Error::Code::BUILD_PROGRAM_FAILURE);
            PROCESS_VAL(Error::Code::COMPILER_NOT_AVAILABLE);
            PROCESS_VAL(Error::Code::INVALID_BINARY);
            PROCESS_VAL(Error::Code::INVALID_BUILD_OPTIONS);
            PROCESS_VAL(Error::Code::INVALID_COMMAND_QUEUE);
            PROCESS_VAL(Error::Code::INVALID_CONTEXT);
            PROCESS_VAL(Error::Code::INVALID_DEVICE);
            PROCESS_VAL(Error::Code::INVALID_EVENT_WAIT_LIST);
            PROCESS_VAL(Error::Code::INVALID_GLOBAL_OFFSET);
            PROCESS_VAL(Error::Code::INVALID_GLOBAL_WORK_SIZE);
            PROCESS_VAL(Error::Code::INVALID_IMAGE_SIZE);
            PROCESS_VAL(Error::Code::INVALID_KERNEL_ARGS);
            PROCESS_VAL(Error::Code::INVALID_ARG_INDEX);
            PROCESS_VAL(Error::Code::INVALID_ARG_VALUE);
            PROCESS_VAL(Error::Code::INVALID_ARG_SIZE);
            PROCESS_VAL(Error::Code::INVALID_KERNEL);
            PROCESS_VAL(Error::Code::INVALID_OPERATION);
            PROCESS_VAL(Error::Code::INVALID_PROGRAM);
            PROCESS_VAL(Error::Code::INVALID_PROGRAM_EXECUTABLE);
            PROCESS_VAL(Error::Code::INVALID_VALUE);
            PROCESS_VAL(Error::Code::INVALID_WORK_DIMENSION);
            PROCESS_VAL(Error::Code::INVALID_WORK_GROUP_SIZE);
            PROCESS_VAL(Error::Code::INVALID_WORK_ITEM_SIZE);
            PROCESS_VAL(Error::Code::INVALID_PLATFORM);
            PROCESS_VAL(Error::Code::MEM_OBJECT_ALLOCATION_FAILURE);
            PROCESS_VAL(Error::Code::MISALIGNED_SUB_BUFFER_OFFSET);
            PROCESS_VAL(Error::Code::OUT_OF_HOST_MEMORY);
            PROCESS_VAL(Error::Code::OUT_OF_RESOURCES);
            PROCESS_VAL(Error::Code::NV_ILLEGAL_READ_OR_WRITE_TO_BUFFER);
            default: s = "Unknown code";
        }
#undef PROCESS_VAL

        return out << s << " (" << static_cast<cl_int>(value) << ")";
    }

    void Error::generateMsg() noexcept {
        std::stringstream ss;
        ss << "OpenCL Error: " << code;
        msg = ss.str();
    }

    const char *Error::what() const noexcept {
        return msg.c_str();
    }

    std::ostream& operator<<(std::ostream& out, const Error &error) {
        return out << error.msg;
    }

    std::ostream& operator<<(std::ostream& out, const Device::Type value) {
        const char* s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
        switch(value){
            PROCESS_VAL(Device::Type::CPU);
            PROCESS_VAL(Device::Type::GPU);
            PROCESS_VAL(Device::Type::ACCELERATOR);
            PROCESS_VAL(Device::Type::CUSTOM);
            PROCESS_VAL(Device::Type::DEFAULT);
            PROCESS_VAL(Device::Type::ALL);
        }
#undef PROCESS_VAL

        return out << s;
    }

    void dumpDeviceInfo(const Device & device) {
        std::cout << "\t              Type: " << device.type << "\n"
                  << "\t              Name: " << device.name << "\n"
                  << "\t            Vendor: " << device.vendor << "\n"
                  << "\t     Image support: " << (device.imageSupport ? "YES" : "NO") << "\n"
                  << "\t    Max image size: " << device.maxImageWidth << "x"
                  << device.maxImageHeight << " pixels\n"
                  << "\t Max compute units: " << device.maxComputeUnits << "\n"
                  << "\tMax work item dims: " << device.maxWorkItemDims << "\n";
    }

    Device getDevice(cl_device_id deviceId) {
        char queryBuffer[1024];
        cl_bool queryBool;
        Device device;
        device.id = deviceId;
        // Name
        cl_int ret = clGetDeviceInfo(deviceId, CL_DEVICE_NAME, 1024, &queryBuffer, nullptr);
        if (ret != CL_SUCCESS) { throw Error(ret); }
        device.name = std::string(queryBuffer);

        // Vendor
        ret = clGetDeviceInfo(deviceId, CL_DEVICE_VENDOR, 1024, &queryBuffer, nullptr);
        if (ret != CL_SUCCESS) { throw Error(ret); }
        device.vendor = std::string(queryBuffer);

        // Type
        ret = clGetDeviceInfo(deviceId, CL_DEVICE_TYPE, 1024, &device.type, nullptr);
        if (ret != CL_SUCCESS) { throw Error(ret); }

        // Image support
        ret = clGetDeviceInfo(deviceId, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &queryBool, nullptr);
        if (ret != CL_SUCCESS) { throw Error(); }
        device.imageSupport = queryBool == CL_TRUE;
        if (device.imageSupport) {
            // Max image width/height
            ret = clGetDeviceInfo(deviceId, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(size_t), &device.maxImageHeight, nullptr);
            if (ret != CL_SUCCESS) { throw Error(ret); }
            ret = clGetDeviceInfo(deviceId, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(size_t), &device.maxImageWidth, nullptr);
            if (ret != CL_SUCCESS) { throw Error(ret); }
        } else {
            device.maxImageWidth = device.maxImageHeight = 0;
        }

        // Max compute units
        ret = clGetDeviceInfo(deviceId, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &device.maxComputeUnits, nullptr);
        if (ret != CL_SUCCESS) { throw Error(ret); }

        // Max work item dims
        ret = clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &device.maxWorkItemDims, nullptr);

        return device;
    }

    std::vector<Device> Platform::getDevices() {
        cl_uint numDevices;
        cl_int ret = clGetDeviceIDs(id, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);
        if (ret != CL_SUCCESS) { throw Error(ret); }
        std::vector<cl_device_id> deviceIds(numDevices);
        ret = clGetDeviceIDs(id, CL_DEVICE_TYPE_ALL, numDevices, deviceIds.data(), nullptr);
        if (ret != CL_SUCCESS) { throw Error(ret); }
        std::vector<Device> devices(numDevices);
        std::transform(deviceIds.begin(), deviceIds.end(), devices.begin(), getDevice);
        return devices;
    }

    Platform getPlatform(cl_platform_id platformId) {
        char queryBuffer[1024];
        Platform platform;
        platform.id = platformId;
        // Name
        cl_int ret = clGetPlatformInfo(platformId, CL_PLATFORM_NAME, 1024, &queryBuffer, NULL);
        if (ret != CL_SUCCESS) { throw Error(ret); }
        platform.name = std::string(queryBuffer);
        // Vendor
        ret = clGetPlatformInfo(platformId, CL_PLATFORM_VENDOR, 1024, &queryBuffer, NULL);
        if (ret != CL_SUCCESS) { throw Error(ret); }
        platform.vendor = std::string(queryBuffer);
        return platform;
    }

    std::vector<Platform> Configurator::getPlatforms() {
        cl_uint numPlatforms;
        cl_int ret = clGetPlatformIDs(0, nullptr, &numPlatforms);
        if (ret != CL_SUCCESS) { throw Error(ret); }
        std::vector<cl_platform_id> platformIds(numPlatforms);
        ret = clGetPlatformIDs(numPlatforms, platformIds.data(), nullptr);
        if (ret != CL_SUCCESS) { throw Error(ret); }
        std::vector<Platform> platforms(numPlatforms);
        std::transform(platformIds.begin(), platformIds.end(), platforms.begin(), getPlatform);
        return platforms;
    }

    void Configurator::dumpPlatforms() {
        std::cout << ">>>> Available OpenCL platforms >>>>\n";
        for (auto platform : getPlatforms()) {
            std::cout << "Platform: " << platform.name << " (" << platform.vendor << ")\n";
            for (auto device : platform.getDevices()) {
                std::cout << "Device: " << "\n";
                dumpDeviceInfo(device);
            }
        }
        std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
    }

    Device Configurator::selectDevice() {
        dumpPlatforms();
        // TODO: Make safer and smarter.
        return getPlatforms()[0].getDevices()[0];
    }

    CommandQueue::CommandQueue() {}
    CommandQueue::CommandQueue(const Context &context) {
        cl_int ret;
        auto queueHandle = clCreateCommandQueue(context.getHandle().get(), context.getDeviceId(), 0, &ret);
        if (ret != CL_SUCCESS) {
            throw Error(ret);
        }
        handle = CommandQueueHandle::takeOwnership(queueHandle);
    }
    CommandQueue::~CommandQueue() {
        if (handle.get()) {
            std::cerr << "[OpenCL] Finalizing command queue\n";
            clFlush(handle.get());
            clFinish(handle.get());
        }
    }
    CommandQueue::CommandQueue(CommandQueue &&other) noexcept : handle(std::move(other.handle)) {}
    CommandQueue &CommandQueue::operator=(CommandQueue &&other) noexcept {
        std::swap(handle, other.handle);
        return *this;
    }

    const CommandQueueHandle &CommandQueue::getHandle() const {
        return handle;
    }

    Context::Context() {}

    Context::Context(cl_context handle) : handle(ContextHandle::retain(handle)) {}

    Context::Context(const Device &device) : device(device) {
        cl_int ret;
        auto contextHandle = clCreateContext(nullptr, 1, &device.id, &clNotify, nullptr, &ret);
        if (contextHandle == 0 || ret != CL_SUCCESS) {
            throw Error(ret);
        }
        handle = ContextHandle::takeOwnership(contextHandle);

        std::cerr << "Created OpenCL context from 1 device:\n";
        dumpDeviceInfo(device);
    }

    Context::Context(Context &&other) noexcept : handle(std::move(other.handle)) {}
    Context &Context::operator=(Context &&other) noexcept {
        std::swap(handle, other.handle);
        return *this;
    }

    const ContextHandle &Context::getHandle() const {
        return handle;
    }

    Device &Context::getDevice() {
        return device;
    }

    cl_device_id Context::getDeviceId() const {
        return device.id;
    }

}
