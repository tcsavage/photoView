#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
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

namespace image {

    class ReshapeException : public Exception {
    public:
        ReshapeException()
            : Exception("Reshape Exception") {}

        ReshapeException(String msg)
            : Exception("Reshape Exception: " + msg) {}
    };

    class ReinterpretException : public Exception {
    public:
        ReinterpretException()
            : Exception("Reinterpret Exception") {}

        ReinterpretException(String msg)
            : Exception("Reinterpret Exception: " + msg) {}
    };

    class Shape {
    public:
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using value_type = size_type;
        using iterator = size_type *;
        using const_iterator = const size_type *;
        using reference = size_type &;
        using const_reference = const size_type &;

        size_type size() const noexcept { return std::accumulate(dims_.begin(), dims_.end(), 1, std::multiplies<size_type>()); }

        iterator begin() { return (iterator)dims_.begin(); }
        const_iterator begin() const { return (const_iterator)dims_.begin(); }
        iterator end() { return dims_.end(); }
        const_iterator end() const { return dims_.end(); }

        reference operator[](size_type idx) {
            return dims_[idx];
        }
        const_reference operator[](size_type idx) const {
            return dims_[idx];
        }

        reference at(size_type idx) {
            return dims_[idx];
        }
        const_reference at(size_type idx) const {
            return dims_[idx];
        }

        const SmallVectorImpl<size_type> &dims() const noexcept { return dims_; }

        void addDim(size_type size) { dims_.pushBack(size); }

        inline bool operator==(const Shape& other) const noexcept {
            if (dims_.size() != other.dims_.size()) {
                return false;
            }
            return std::equal(dims_.begin(), dims_.end(), other.dims_.begin());
        }

        Shape() : dims_() {}
        Shape(std::initializer_list<size_type> dims) : dims_(dims) {}

    private:
        SmallVector<size_type, 4> dims_;
    };

    inline std::ostream &operator<<(std::ostream &output, const Shape &shape) {
        output << "{";
        std::copy(shape.begin(), shape.end(), std::ostream_iterator<Shape::size_type>(output, ", "));
        return output << "}";
    }

    using NDArrayStoragePtr = std::shared_ptr<void>;

    struct NDArrayStorage {
        NDArrayStoragePtr ptr;

        template <class T>
        T *getPtr() { return static_cast<T*>(ptr.get()); }

        template <class T>
        const T *getPtr() const { return static_cast<T*>(ptr.get()); }

        NDArrayStorage(std::size_t size, std::size_t alignment)
            : ptr(std::aligned_alloc(alignment, size), [](auto p) { std::free(p); }) {}

        explicit NDArrayStorage(NDArrayStoragePtr ptr) : ptr(ptr) {}
    };

    inline Shape shapeStride(const Shape& shape) {
        Shape strides = shape;
        std::exclusive_scan(shape.begin(), shape.end(), strides.begin(), 1, std::multiplies<Shape::size_type>());
        return strides;
    }

    class NDArrayBase : protected NDArrayStorage {
    public:
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        
        size_type size() const noexcept { return shape_.size(); }
        Shape shape() const noexcept { return shape_; }
        Shape strides() const noexcept { return strides_; }

        size_type sizeBytes() const noexcept { return size() * type_->size(); }

        size_type shapeOffset(const Shape& idx) const noexcept {
            return std::inner_product(idx.begin(), idx.end(), strides_.begin(), 0);
        }

        NDArrayBase(TypeRef type, Shape shape, size_type alignment = 0)
            : NDArrayStorage(type->size() * shape.size(), alignment ? alignment : type->alignment())
            , shape_(shape)
            , strides_(shapeStride(shape_))
            , type_(type) {}

    protected:
        NDArrayBase(TypeRef type, Shape shape, NDArrayStoragePtr ptr)
            : NDArrayStorage(ptr)
            , shape_(shape)
            , strides_(shapeStride(shape_))
            , type_(type) {}

    private:
        Shape shape_;
        Shape strides_;
        TypeRef type_;
    };

    template <class T>
    class NDArray : public NDArrayBase {
        template <class U>
        friend class NDArray;
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

        iterator begin() { return (iterator)this->getPtr<T>(); }
        const_iterator begin() const { return (const_iterator)this->getPtr<T>(); }
        iterator end() { return begin() + size(); }
        const_iterator end() const { return begin() + size(); }

        pointer data() { return pointer(begin()); }
        const_pointer data() const { return const_pointer(begin()); }

        reference operator[](size_type idx) {
            assert(idx < size());
            return begin()[idx];
        }
        const_reference operator[](size_type idx) const {
            assert(idx < size());
            return begin()[idx];
        }

        reference at(size_type idx) {
            assert(idx < size());
            return begin()[idx];
        }
        const_reference at(size_type idx) const {
            assert(idx < size());
            return begin()[idx];
        }

        reference operator[](Shape idx) {
            return begin()[shapeOffset(idx)];
        }
        const_reference operator[](Shape idx) const {
            return begin()[shapeOffset(idx)];
        }

        reference at(Shape idx) {
            return begin()[shapeOffset(idx)];
        }
        const_reference at(Shape idx) const {
            return begin()[shapeOffset(idx)];
        }

        template <class U>
        NDArray<U> reinterpret() {
            return NDArray<U>(Shape { sizeBytes() / sizeof(U) }, ptr);
        }

        explicit NDArray(Shape shape, size_type alignment = 0) : NDArrayBase(RepresentType<T>::get(), shape, alignment) {}
        explicit NDArray(std::initializer_list<Shape::size_type> dims) : NDArray(Shape(dims)) {}
        explicit NDArray() : NDArray(Shape()) {}

    private:
        NDArray(Shape shape, NDArrayStoragePtr ptr) : NDArrayBase(RepresentType<T>::get(), shape, ptr) {}
    };

}
