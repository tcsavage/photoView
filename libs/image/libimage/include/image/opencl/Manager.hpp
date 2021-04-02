#pragma once

#include <map>
#include <memory>

#include <image/Expected.hpp>

#include <image/opencl/BufferDevice.hpp>
#include <image/opencl/Context.hpp>
#include <image/opencl/Manager.hpp>
#include <image/opencl/Types.hpp>

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

        Expected<Program, Error> programFromResource(const String &filename) noexcept;

        std::map<String, Program> programs;

        static Manager *the() noexcept;

        Manager() noexcept;

        ~Manager() noexcept;
    };

}
