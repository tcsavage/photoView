#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <utility>

namespace image::memory {

    using Size = std::size_t;
    using PtrVal = std::intptr_t;

    constexpr bool isAligned(const void *ptr, Size alignment) noexcept {
        auto iptr = reinterpret_cast<Size>(ptr);
        return !(iptr % alignment);
    }

    constexpr Size roundToAlignment(Size x, Size alignment) {
        return (x + alignment) & ~alignment;
    }

    template <class T>
    constexpr T *roundToAlignment(T *x, Size alignment) {
        return reinterpret_cast<void*>((reinterpret_cast<Size>(x) + alignment) & ~alignment);
    }

    struct Block {
        void *ptr { nullptr };
        Size size { 0 };

        constexpr explicit operator bool() const noexcept {
            return ptr != nullptr;
        }
    };

    constexpr Block nullBlock { };

    template <Size BlockSize, Size Alignment = alignof(std::max_align_t)>
    struct alignas(Alignment) BlockObj {
        char arr_[BlockSize];

        constexpr Block get() noexcept {
            return Block { &arr_[0], BlockSize };
        }
    };

    template <class Alloc>
    concept Allocator = requires(Alloc alloc, Size size, Size alignment, Block block) {
        { Alloc::goodSize(size) } noexcept -> std::same_as<Size>;
        // { Alloc::alignment } noexcept -> std::same_as<Size>;
        { alloc.alloc(size) } noexcept -> std::same_as<Block>;
        { alloc.alignedAlloc(size, alignment) } noexcept -> std::same_as<Block>;
        { alloc.free(block) } noexcept -> std::same_as<void>;
    };

    template <class Alloc>
    concept OwningAllocator = Allocator<Alloc> && requires(Alloc alloc, Size size, Size alignment, Block block) {
        { alloc.owns(block) } noexcept -> std::same_as<bool>;
    };

    template <class Alloc>
    concept ChunkingAllocator = OwningAllocator<Alloc> && requires(Alloc alloc, Size size, Size alignment, Block block) {
        { alloc.allocAll() } noexcept -> std::same_as<Block>;
        { alloc.freeAll() } noexcept -> std::same_as<void>;
    };

    template <class T, Allocator Alloc, class... Args>
    T *typedAlloc(Alloc &alloc, Args&& ... args) {
        auto block = alloc.alloc(sizeof(T));
        void *ptr = new (block.ptr) T;
        return reinterpret_cast<T*>(ptr);
    }

    struct AbstractAllocator {
        virtual Block alloc(Size size) noexcept = 0;
        virtual Block alignedAlloc(Size size, Size alignment) noexcept = 0;
        virtual void free(Block block) noexcept = 0;

        virtual ~AbstractAllocator() {}
    };

    template <Allocator Alloc>
    struct AnyAllocator : public AbstractAllocator {
        virtual Block alloc(Size size) noexcept override {
            return alloc_.alloc(size);
        };

        virtual Block alignedAlloc(Size size, Size alignment) noexcept override {
            return alloc_.alignedAlloc(size, alignment);
        };

        virtual void free(Block block) noexcept override {
            alloc_.free(block);
        };

        constexpr AnyAllocator(Alloc &alloc) noexcept : alloc_(alloc) {}
        virtual ~AnyAllocator() {}

        Alloc &alloc_;
    };

    struct MallocAllocator {
        constexpr static Size alignment = alignof(std::max_align_t);
        constexpr static Size goodSize(Size size) noexcept { return size; }

        Block alloc(Size size) noexcept;
        Block alignedAlloc(Size size, Size alignment) noexcept;
        void free(Block block) noexcept;
    };


    template <Size Alignment = alignof(std::max_align_t)>
    struct StackAllocator {
        Block block_;
        PtrVal top_;

        StackAllocator(Block block) noexcept
            : block_(block)
            , top_(reinterpret_cast<PtrVal>(roundToAlignment(block.ptr, Alignment))) {}

        constexpr static Size alignment = Alignment;
        constexpr static Size goodSize(Size size) noexcept { return roundToAlignment(size, alignment); }

        Block alloc(Size size) noexcept {
            auto size1 = roundToAlignment(size, alignment);
            if (size1 > (reinterpret_cast<PtrVal>(block_.ptr) + block_.size) - top_) {
                return nullBlock;
            }
            Block result { reinterpret_cast<void *>(top_), size };
            top_ += size1;
            return result;
        }

        Block alignedAlloc(Size size, Size alignment) noexcept { return nullBlock; }

        void free(Block block) noexcept {
            if (reinterpret_cast<PtrVal>(block.ptr) + roundToAlignment(block.size, alignment) == top_) {
                top_ = reinterpret_cast<PtrVal>(block.ptr);
            }
        }

        bool owns(Block block) const noexcept {
            return block.ptr >= block_.ptr && block.ptr < block_.ptr + block_.size;
        }
    };

    template <OwningAllocator Primary, Allocator Fallback>
    struct FallbackAllocator {
        Primary &primary;
        Fallback &fallback;

        explicit FallbackAllocator(Primary &p, Fallback &f) : primary(p), fallback(f) {}

        constexpr static Size alignment = Primary::alignment;
        constexpr static Size goodSize(Size size) noexcept { return Primary::goodSize(size); }

        Block alloc(Size size) noexcept {
            Block b = nullBlock;
            primary.alloc(size);
            if (!b) {
                fallback.alloc(size);
            }
            return b;
        }

        Block alignedAlloc(Size size, Size alignment) noexcept {
            Block b = nullBlock;
            primary.alignedAlloc(size, alignment);
            if (!b) {
                fallback.alignedAlloc(size, alignment);
            }
            return b;
        }

        void free(Block block) noexcept {
            if (primary.owns(block)) {
                primary.free(block);
            } else {
                fallback.free(block);
            }
        }

        bool owns(Block block) const noexcept {
            return primary.owns(block) || fallback.owns(block);
        }
    };

    namespace {
        inline MallocAllocator defaultAllocatorImpl;
    }

    inline AnyAllocator<MallocAllocator> defaultAllocator { defaultAllocatorImpl };

}
