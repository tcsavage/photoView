#include <algorithm>
#include <iostream>
#include <ranges>

#include <image/NDArray.hpp>
#include <image/memory/Buffer.hpp>
#include <image/opencl/Manager.hpp>

using namespace image;
using namespace image::memory;

struct HexByte {
    U8 byte;
};

inline std::ostream &operator<<(std::ostream &o, const HexByte &hb) {
    return o << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hb.byte);
}

void dumpHexTable(const Block &block) {
    HexByte *ptr = reinterpret_cast<HexByte *>(block.ptr);
    Size width = 0x10;
    std::cerr << "== HEXDUMP =====================================================\n";
    std::cerr << std::hex << std::setfill('0');
    for (Size i = 0; i < block.size; ++i) {
        if (i % width == 0) {
            if (i > 0) { std::cerr << "\n"; }
            std::cerr << ptr + i << " |";
        }
        std::cerr << " " << ptr[i];
    }
    std::cerr << std::dec << "\n";
    std::cerr << "================================================================\n";
}

int main(int, const char *[]) {
    opencl::Manager oclMan;

    // Create a block of memory and a stack allocator to parcel it out.
    BlockObj<128> mem;
    StackAllocator<> allocImpl { mem.get() };
    AnyAllocator<StackAllocator<>> alloc { allocImpl };

    // Create a non-owning buffer and array on the entire block of memory.
    // This is so that we can observe the whole thing.
    auto allBytesBuf = makeSharedBuffer(mem.get());
    NDArray<U8> allBytesArr { Shape { 128 }, allBytesBuf };
    std::fill(std::begin(allBytesArr), std::end(allBytesArr), 0);

    // Create a shared buffer with the stack allocator, then an array using that block.
    auto buf1 = makeSharedBuffer(4, &alloc);
    buf1->malloc();
    NDArray<U8> arr1 { Shape { 2, 2 }, buf1 };
    std::cerr << "arr1: " << arr1.strides() << "\n";
    std::fill(std::begin(arr1), std::end(arr1), 0xFE);

    // Create an array which allocates its own buffer from the stack allocator.
    NDArray<U8> arr2 { Shape { 2, 2 }, 0, &alloc };
    std::fill(std::begin(arr2), std::end(arr2), 0xCD);

    dumpHexTable(mem.get());

    return 0;
}
