#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>

#include <image/CoreTypes.hpp>
#include <image/Exception.hpp>
#include <image/SmallVector.hpp>
#include <image/Type.hpp>
#include <image/memory/Buffer.hpp>

namespace image {

    namespace detail {
        constexpr auto roundUpToMultiple(auto n, auto multiple) { return ((n + multiple - 1) / multiple) * multiple; }
    }

    /**
     * @brief Represents the N-dimensional size of an NDArray.
     *
     * Is also used to represent an N-dimensional index into an NDArray.
     */
    class Shape {
    public:
        using size_type = memory::Size;
        using difference_type = std::ptrdiff_t;
        using value_type = size_type;
        using iterator = size_type *;
        using const_iterator = const size_type *;
        using reference = size_type &;
        using const_reference = const size_type &;

        size_type size() const noexcept {
            return std::accumulate(dims_.begin(), dims_.end(), 1, std::multiplies<size_type>());
        }

        iterator begin() noexcept { return (iterator)dims_.begin(); }
        const_iterator begin() const noexcept { return (const_iterator)dims_.begin(); }
        iterator end() noexcept { return dims_.end(); }
        const_iterator end() const noexcept { return dims_.end(); }

        reference operator[](size_type idx) noexcept { return dims_[idx]; }
        const_reference operator[](size_type idx) const noexcept { return dims_[idx]; }

        reference at(size_type idx) noexcept { return dims_[idx]; }
        const_reference at(size_type idx) const noexcept { return dims_[idx]; }

        const SmallVectorImpl<size_type> &dims() const noexcept { return dims_; }

        void addDim(size_type size) { dims_.pushBack(size); }

        inline bool operator==(const Shape &other) const noexcept {
            if (dims_.size() != other.dims_.size()) { return false; }
            return std::equal(dims_.begin(), dims_.end(), other.dims_.begin());
        }

        Shape() noexcept : dims_() {}
        explicit Shape(std::initializer_list<size_type> dims) noexcept : dims_(dims) {}

    private:
        SmallVector<size_type, 4> dims_;
    };

    namespace detail {
        // Calculates the strides of each dimension in a shape.
        inline Shape shapeStride(const Shape &shape) noexcept {
            Shape strides = shape;
            std::exclusive_scan(shape.begin(), shape.end(), strides.begin(), 1, std::multiplies<Shape::size_type>());
            return strides;
        }
    }

    inline std::ostream &operator<<(std::ostream &output, const Shape &shape) noexcept {
        output << "{";
        std::copy(shape.begin(), shape.end(), std::ostream_iterator<Shape::size_type>(output, ", "));
        return output << "}";
    }

    /**
     * @brief Manages the backend storage of an NDArray.
     *
     * NDArrays are backed by a ref-counted SharedBuffer. This means that an
     * NDArray can be copied and both objects will point to the same memory.
     */
    struct NDArrayStorage {
        void *bufferHostPtr { nullptr };  // This must be kept in-sync with buffer->data()
        memory::Size size;
        memory::SharedBuffer buffer;  // Maintains the ref-counted reference to the underlying buffer

        template <class T>
        constexpr T *getPtr() noexcept {
            return static_cast<T *>(bufferHostPtr);
        }

        template <class T>
        constexpr const T *getPtr() const noexcept {
            return static_cast<T *>(bufferHostPtr);
        }

        explicit NDArrayStorage(memory::Size size, memory::Size alignment, memory::AbstractAllocator *alloc) noexcept
          : size(size)
          , buffer(alloc == nullptr ? memory::makeSharedBuffer(size, alignment)
                                    : memory::makeSharedBuffer(size, alignment, alloc)) {
            buffer->malloc();
            bufferHostPtr = buffer->data();
            std::cerr << "[NDArray] Created new storage - size 0x" << std::hex << size << " at [" << bufferHostPtr
                      << ", " << reinterpret_cast<void *>(reinterpret_cast<intptr_t>(bufferHostPtr) + size) << ")\n"
                      << std::dec;
        }

        explicit NDArrayStorage(memory::Size size, memory::Size alignment) noexcept
          : NDArrayStorage(size, alignment, nullptr) {}

        explicit NDArrayStorage(memory::SharedBuffer buffer) noexcept
          : bufferHostPtr(buffer->data())
          , size(buffer->size)
          , buffer(buffer) {}
    };

    /**
     * @brief Untyped base class for NDArray providing basic functions.
     *
     * Stores the shape of the array, as well as stride and type information.
     */
    class NDArrayBase : protected NDArrayStorage {
    public:
        using size_type = memory::Size;
        using difference_type = std::ptrdiff_t;

        size_type size() const noexcept { return shape_.size(); }
        Shape shape() const noexcept { return shape_; }
        Shape strides() const noexcept { return strides_; }

        size_type sizeBytes() const noexcept { return size() * type_->size(); }

        /**
         * @brief Calculates a byte offset into the array for the element identified by the shape index.
         */
        size_type shapeOffset(const Shape &idx) const noexcept {
            return std::inner_product(idx.begin(), idx.end(), strides_.begin(), 0);
        }

        /**
         * @brief Calculates a byte offset into the array for the element identified by the coordinates.
         */
        template <class... Ts>
        constexpr size_type dimsOffset(Ts &&... dims) const noexcept {
            std::array<size_type, sizeof...(Ts)> dimsArr { static_cast<size_type>(dims)... };
            return std::inner_product(dimsArr.begin(), dimsArr.end(), strides_.begin(), 0);
        }

        memory::SharedBuffer buffer() noexcept { return NDArrayStorage::buffer; }
        const memory::SharedBuffer buffer() const noexcept { return NDArrayStorage::buffer; }

        TypeRef type() const noexcept { return type_; }

        NDArrayBase(TypeRef type,
                    Shape shape,
                    size_type alignment = 0,
                    memory::AbstractAllocator *alloc = nullptr) noexcept
          : NDArrayStorage(type->size() * shape.size(), alignment ? alignment : type->alignment(), alloc)
          , shape_(shape)
          , strides_(detail::shapeStride(shape_))
          , type_(type) {}

    protected:
        NDArrayBase(TypeRef type, Shape shape, memory::SharedBuffer buffer) noexcept
          : NDArrayStorage(buffer)
          , shape_(shape)
          , strides_(detail::shapeStride(shape_))
          , type_(type) {}

    private:
        Shape shape_;
        Shape strides_;
        TypeRef type_;
    };

    /**
     * @brief N-dimensional array
     */
    template <std::semiregular T>
    class NDArray : public NDArrayBase {
        // template <class U>
        // friend class NDArray;
    public:
        using value_type = T;
        using iterator = T *;
        using const_iterator = const T *;

        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using reverse_iterator = std::reverse_iterator<iterator>;

        using reference = T &;
        using const_reference = const T &;
        using pointer = T *;
        using const_pointer = const T *;

        iterator begin() noexcept { return (iterator)getPtr<T>(); }
        const_iterator begin() const noexcept { return (const_iterator)getPtr<T>(); }
        iterator end() noexcept { return begin() + size(); }
        const_iterator end() const noexcept { return begin() + size(); }

        pointer data() noexcept { return pointer(begin()); }
        const_pointer data() const noexcept { return const_pointer(begin()); }

        reference operator[](size_type idx) noexcept {
            assert(idx < size());
            return begin()[idx];
        }
        const_reference operator[](size_type idx) const noexcept {
            assert(idx < size());
            return begin()[idx];
        }

        reference at(size_type idx) noexcept {
            assert(idx < size());
            return begin()[idx];
        }
        const_reference at(size_type idx) const noexcept {
            assert(idx < size());
            return begin()[idx];
        }

        reference operator[](Shape idx) noexcept { return begin()[shapeOffset(idx)]; }
        const_reference operator[](Shape idx) const noexcept { return begin()[shapeOffset(idx)]; }

        reference at(Shape idx) noexcept { return begin()[shapeOffset(idx)]; }
        const_reference at(Shape idx) const noexcept { return begin()[shapeOffset(idx)]; }

        template <class... Ts>
        reference at(Ts &&... dims) noexcept {
            return begin()[dimsOffset(std::forward<Ts>(dims)...)];
        }
        template <class... Ts>
        const_reference at(Ts &&... dims) const noexcept {
            return begin()[dimsOffset(std::forward<Ts>(dims)...)];
        }

        template <class U>
        NDArray<U> reinterpret() noexcept {
            return NDArray<U>(Shape { sizeBytes() / sizeof(U) }, NDArrayStorage::buffer);
        }

        NDArray reshape(Shape s) noexcept {
            assert(s.size() == size());
            return NDArray(s, NDArrayStorage::buffer);
        }

        explicit NDArray(Shape shape, size_type alignment = 0, memory::AbstractAllocator *alloc = nullptr) noexcept
          : NDArrayBase(RepresentType<T>::get(), shape, alignment, alloc) {}
        explicit NDArray(Shape shape,
                         Shape alignments,
                         size_type alignment = 0,
                         memory::AbstractAllocator *alloc = nullptr) noexcept
          : NDArrayBase(RepresentType<T>::get(), shape, alignments, alignment, alloc) {}
        explicit NDArray(std::initializer_list<Shape::size_type> dims) noexcept : NDArray(Shape(dims)) {}
        explicit NDArray() noexcept : NDArray(Shape()) {}
        explicit NDArray(Shape shape, memory::SharedBuffer buffer) noexcept
          : NDArrayBase(RepresentType<T>::get(), shape, buffer) {
            assert(shape.size() * type()->size() <= buffer->size);
        }
    };

}
