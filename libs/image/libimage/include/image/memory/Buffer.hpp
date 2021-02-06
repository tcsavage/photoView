#pragma once

#include <cassert>
#include <cstddef>
#include <memory>

#include <image/memory/Allocator.hpp>

namespace image::memory {

    struct Buffer;

    struct AbstractDevice {
        Size size;

        virtual void malloc(Buffer &) noexcept = 0;
        virtual void free(Buffer &) noexcept = 0;
        virtual void sync(Buffer &) noexcept = 0;
        virtual void copyDeviceToHost(Buffer &) noexcept = 0;
        virtual void copyHostToDevice(Buffer &) noexcept = 0;

        virtual ~AbstractDevice() noexcept {}
    };

    struct Buffer {
        Block hostBlock;
        Size size { 0 };
        Size alignment { 0 };
        std::shared_ptr<AbstractDevice> device;
        std::intptr_t deviceHandle { 0 };
        bool ownsHostBlock { false };
        AbstractAllocator *allocator { &defaultAllocator };

        constexpr void *data() noexcept { return hostBlock.ptr; }

        inline void malloc() noexcept {
            hostBlock = allocator->alloc(size);
            ownsHostBlock = true;
        }

        inline void free() noexcept {
            assert(ownsHostBlock);
            allocator->free(hostBlock);
        }

        inline void setDevice(std::shared_ptr<AbstractDevice> dev) noexcept {
            assert(device == nullptr);
            device = dev;
        }

        inline void deviceMalloc() noexcept {
            assert(device);
            device->malloc(*this);
        }

        inline void deviceFree() noexcept {
            assert(device);
            device->free(*this);
        }

        inline void copyDeviceToHost() noexcept {
            assert(device);
            assert(hostBlock);
            device->copyDeviceToHost(*this);
        }

        inline void copyHostToDevice() noexcept {
            assert(device);
            assert(hostBlock);
            device->copyHostToDevice(*this);
        }

        constexpr explicit Buffer() noexcept {}

        constexpr explicit Buffer(Block block) noexcept : hostBlock(block), size(block.size), allocator(nullptr) {}
        constexpr explicit Buffer(Size size) noexcept : size(size) {}
        constexpr explicit Buffer(Size size, AbstractAllocator *alloc) noexcept : size(size), allocator(alloc) {}
        constexpr explicit Buffer(Size size, Size alignment) noexcept : size(size), alignment(alignment) {}
        constexpr explicit Buffer(Size size, Size alignment, AbstractAllocator *alloc) noexcept
          : size(size)
          , alignment(alignment)
          , allocator(alloc) {}

        ~Buffer() noexcept {
            if (device) { deviceFree(); }
            if (hostBlock && ownsHostBlock) { free(); }
        }
    };

    using SharedBuffer = std::shared_ptr<Buffer>;

    inline SharedBuffer makeSharedBuffer(Block block) noexcept { return std::make_shared<Buffer>(block); }
    inline SharedBuffer makeSharedBuffer(Size size) noexcept { return std::make_shared<Buffer>(size); }
    inline SharedBuffer makeSharedBuffer(Size size, Size alignment) noexcept {
        return std::make_shared<Buffer>(size, alignment);
    }
    inline SharedBuffer makeSharedBuffer(Size size, AbstractAllocator *alloc) noexcept {
        return std::make_shared<Buffer>(size, alloc);
    }
    inline SharedBuffer makeSharedBuffer(Size size, Size alignment, AbstractAllocator *alloc) noexcept {
        return std::make_shared<Buffer>(size, alignment, alloc);
    }

}
