#pragma once

#include <cstddef>
#include <memory>

#include <image/memory/Allocator.hpp>

namespace image::memory {

    struct Buffer;

    struct AbstractDevice {
        std::size_t size;

        virtual void malloc(Buffer &) noexcept = 0;
        virtual void free(Buffer &) noexcept = 0;
        virtual void sync(Buffer &) noexcept = 0;
        virtual void copyDeviceToHost(Buffer &) noexcept = 0;
        virtual void copyHostToDevice(Buffer &) noexcept = 0;

        virtual ~AbstractDevice() noexcept {}
    };

    struct Buffer {
        Block hostBlock;
        std::size_t size { 0 };
        std::size_t alignment { 0 };
        std::shared_ptr<AbstractDevice> device;
        std::intptr_t deviceHandle { 0 };
        bool hostOwnsPtr { false };
        AbstractAllocator *allocator { &defaultAllocator };

        constexpr void *data() noexcept { return hostBlock.ptr; }

        inline void malloc() noexcept {
            hostBlock = allocator->alloc(size);
        }

        inline void free() noexcept {
            allocator->free(hostBlock);
        }

        inline void setDevice(std::shared_ptr<AbstractDevice> dev) noexcept {
            device = dev;
        }

        inline void deviceMalloc() noexcept {
            device->malloc(*this);
        }

        inline void deviceFree() noexcept {
            device->free(*this);
        }

        inline void copyDeviceToHost() noexcept {
            device->copyDeviceToHost(*this);
        }

        inline void copyHostToDevice() noexcept {
            device->copyHostToDevice(*this);
        }

        explicit Buffer() noexcept {}

        explicit Buffer(std::size_t size) noexcept : size(size) {}
        explicit Buffer(std::size_t size, std::size_t alignment) noexcept : size(size), alignment(alignment) {}

        ~Buffer() noexcept {
            if (device) {
                deviceFree();
            }
            free();
        }
    };

}
