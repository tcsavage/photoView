#include <image/memory/Allocator.hpp>

namespace image::memory {

    Block MallocAllocator::alloc(Size size) noexcept {
        auto ptr = std::malloc(size);
        return Block { ptr, size };
    }

    Block MallocAllocator::alignedAlloc(Size size, Size alignment) noexcept {
        auto ptr = std::aligned_alloc(alignment, size);
        return Block { ptr, size };
    }

    void MallocAllocator::free(Block block) noexcept {
        std::free(block.ptr);
    }

}
