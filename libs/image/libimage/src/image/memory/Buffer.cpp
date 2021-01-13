#include <image/memory/Buffer.hpp>

#include <cstdlib>
#include <iostream>

namespace image::memory {

    void DefaultAllocator::malloc(Buffer &buf) noexcept {
        if (buf.alignment > 0) {
            std::cerr << "[DefaultAllocator] Aligned allocation (" << buf.alignment
                      << ") of " << std::hex << buf.size << std::dec << " bytes\n";
            buf.hostPtr = std::aligned_alloc(buf.alignment, buf.size);
        } else {
            std::cerr << "[DefaultAllocator] Unaligned (default) allocation of " << std::hex << buf.size << std::dec << " bytes\n";
            buf.hostPtr = std::malloc(buf.size);
            buf.alignment = sizeof(max_align_t);
        }
        std::cerr << "[DefaultAllocator] Allocation returned ptr: " << std::hex << buf.hostPtr << std::dec << "\n";
        buf.hostOwnsPtr = true;
    }

    void DefaultAllocator::free(Buffer &buf) noexcept {
        std::free(buf.hostPtr);
        buf.hostPtr = nullptr;
        buf.hostOwnsPtr = false;
    }

}
