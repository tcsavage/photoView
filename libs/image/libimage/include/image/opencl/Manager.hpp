#pragma once

#include <memory>

#include <image/opencl/BufferDevice.hpp>
#include <image/opencl/Context.hpp>

namespace image::opencl {

    /**
     * @brief Singleton type for managing global OpenCL state.
     * 
     * Create a single instance (ideally in main()) and reference with Manager::the().
     */
    struct Manager {
        Configurator config;
        Context context;
        CommandQueue queue;
        std::shared_ptr<memory::OpenCLDevice> bufferDevice;

        static Manager *the() noexcept;

        Manager() noexcept;

        ~Manager() noexcept;
    };

}
