#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <utility>

#include <image/Util.hpp>

namespace image {

    using SmallVectorSize = std::size_t;

    template <class T>
    struct AlignedBuffer {
        alignas(T) char buffer[sizeof(T)];

        T* get() noexcept { return reinterpret_cast<T*>(&buffer[0]); }
    };

    class SmallVectorBase {
    public:
        SmallVectorSize size() const noexcept { return size_; }
        SmallVectorSize capacity() const noexcept { return capacity_; }
        bool empty() const noexcept { return !size_; }

    protected:
        void *bufferPtr;
        SmallVectorSize size_ { 0 };
        SmallVectorSize capacity_ { 0 };
        // bool usesInlineBuffer {true};

        SmallVectorBase() = delete;
        SmallVectorBase(void *ptr, SmallVectorSize capacity) : bufferPtr(ptr), capacity_(capacity) {}

        void setSize(SmallVectorSize N) {
            assert(N <= capacity());
            size_ = N;
        }
    };

    template <class T>
    struct SmallVectorAlignmentAndSize {
        AlignedBuffer<SmallVectorBase> base;
        AlignedBuffer<T> firstEl;
    };

    template <class T>
    class SmallVectorImpl : public SmallVectorBase {
    public:
        using size_type = SmallVectorSize;
        using difference_type = ptrdiff_t;
        using value_type = T;
        using iterator = T *;
        using const_iterator = const T *;
        
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using reverse_iterator = std::reverse_iterator<iterator>;
        
        using reference = T &;
        using const_reference = const T &;
        using pointer = T *;
        using const_pointer = const T *;

        size_type size() const { return size_; }
        size_type capacity() const { return capacity_; }
        bool empty() const { return !size_; }

        iterator begin() { return (iterator)this->bufferPtr; }
        const_iterator begin() const { return (const_iterator)this->bufferPtr; }
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
        
        reference front() {
            assert(!empty());
            return begin()[0];
        }
        const_reference front() const {
            assert(!empty());
            return begin()[0];
        }
        
        reference back() {
            assert(!empty());
            return end()[-1];
        }
        const_reference back() const {
            assert(!empty());
            return end()[-1];
        }

        void clear() {
            this->destroyRange(this->begin(), this->end());
            this->size_ = 0;
        }

        void resize(size_type N) {
            if (N < this->size()) {
                this->destroyRange(this->begin() + N, this->end());
                this->setSize(N);
            } else if (N > this->size()) {
                if (this->capacity() < N) {
                    this->grow(N);
                }
                for (auto it = this->end(), e = this->begin() + N; it != e; ++it) {
                    new (&*it) T();
                }
                this->setSize(N);
            }
        }

        void resize(size_type N, const T &value) {
            if (N < this->size()) {
                this->destroyRange(this->begin() + N, this->end());
                this->setSize(N);
            } else if (N > this->size()) {
                if (this->capacity() < N) {
                    this->grow(N);
                }
                std::uninitialized_fill(this->end(), this->begin() + N, value);
                this->setSize(N);
            }
        }

        void reserve(size_type N) {
            if (this->capacity() < N) {
                this->grow(N);
            }
        }

        void pushBack(const T &value) {
            if (this->size() >= this->capacity()) {
                // Unlikely.
                this->grow();
            }
            new ((void*) this->end()) T(value);
            this->setSize(this->size() + 1);
        }

        void pushBack(T &&value) {
            if (this->size() >= this->capacity()) {
                // Unlikely.
                this->grow();
            }
            new ((void*) this->end()) T(std::move(value));
            this->setSize(this->size() + 1);
        }

        T popBack() {
            T out = std::move(this->back());
            this->popBack_();
            return out;
        }

        template <typename InIter>
        void append(InIter inBegin, InIter inEnd) {
            size_type N = std::distance(inBegin, inEnd);
            if (N > this->capacity() - this->size()) {
                this->grow(this->size() + N);
            }
            std::uninitialized_copy(inBegin, inEnd, this->end());
            this->setSize(this->size() + N);
        }

        void assign(size_type N, const T &value) {
            clear();
            if (this->capacity() < N) {
                this->grow(N);
            }
            this->setSize(N);
            std::uninitialized_fill(this->begin(), this->end(), value);
        }

        void assign(std::initializer_list<T> il) {
            clear();
            append(il.begin(), il.end());
        }

        SmallVectorImpl &operator=(const SmallVectorImpl &rhs);
 
        SmallVectorImpl &operator=(SmallVectorImpl &&rhs);

        bool isSmall() const noexcept { return this->getFirstEl() == this->begin(); }

        bool isOnHeap() const noexcept { return !(this->getFirstEl() == this->begin()); }

        void resetToSmall() {
            this->bufferPtr = getFirstEl();
            this->size_ = this->capacity_ = 0;
        }

        ~SmallVectorImpl() {
            if (!this->isSmall()) {
                free(this->begin());
            }
        }

    protected:
        void destroyRange(T *b, T *e) {
            while (b != e) {
                --e;
                e->~T();
            }
        }

        void grow(size_type minSize = 0);

        void popBack_() {
            this->setSize(this->size() - 1);
            this->end()->~T();
        }

        explicit SmallVectorImpl(size_type N) : SmallVectorBase(getFirstEl(), N) {}

    private:
        void *getFirstEl() const {
            return const_cast<void *>(reinterpret_cast<const void *>(reinterpret_cast<const char *>(this) + offsetof(SmallVectorAlignmentAndSize<T>, firstEl)));
        }
    };

    template <typename T>
    void SmallVectorImpl<T>::grow(SmallVectorSize minSize) {
        // TODO: Max size?
        SmallVectorSize newCapacity = SmallVectorSize(nextPowerOf2(this->capacity() + 2));
        newCapacity = std::max(newCapacity, minSize);

        T *newBuffer = static_cast<T*>(malloc(newCapacity * sizeof(T)));
        std::uninitialized_move(this->begin(), this->end(), newBuffer);
        this->destroyRange(this->begin(), this->end());
        if (!this->isSmall()) {
            free(this->begin());
        }

        this->bufferPtr = newBuffer;
        this->capacity_ = newCapacity;
        // this->usesInlineBuffer = false;
    }

    template <typename T>
    SmallVectorImpl<T> &SmallVectorImpl<T>::operator=(const SmallVectorImpl<T> &rhs) {
        // Avoid self-assignment.
        if (this == &rhs) return *this;

        // If we already have sufficient space, assign the common elements, then
        // destroy any excess.
        SmallVectorSize rhsSize = rhs.size();
        SmallVectorSize curSize = this->size();
        if (curSize >= rhsSize) {
            // Assign common elements.
            iterator newEnd;
            if (rhsSize)
            newEnd = std::copy(rhs.begin(), rhs.begin()+rhsSize, this->begin());
            else
            newEnd = this->begin();

            // Destroy excess elements.
            this->destroyRange(newEnd, this->end());

            // Trim.
            this->setSize(rhsSize);
            return *this;
        }

        // If we have to grow to have enough elements, destroy the current elements.
        // This allows us to avoid copying them during the grow.
        // FIXME: don't do this if they're efficiently moveable.
        if (this->capacity() < rhsSize) {
            // Destroy current elements.
            this->destroyRange(this->begin(), this->end());
            this->setSize(0);
            curSize = 0;
            this->grow(rhsSize);
        } else if (curSize) {
            // Otherwise, use assignment for the already-constructed elements.
            std::copy(rhs.begin(), rhs.begin()+curSize, this->begin());
        }

        // Copy construct the new elements in place.
        std::uninitialized_copy(rhs.begin() + curSize, rhs.end(), this->begin() + curSize);

        // Set end.
        this->setSize(rhsSize);
        return *this;
    }

    template <typename T>
    SmallVectorImpl<T> &SmallVectorImpl<T>::operator=(SmallVectorImpl<T> &&rhs) {
        // Avoid self-assignment.
        if (this == &rhs) return *this;

        // If the rhs isn't small, clear this vector and then steal its buffer.
        if (!rhs.isSmall()) {
            this->destroyRange(this->begin(), this->end());
            if (!this->isSmall()) free(this->begin());
            this->bufferPtr = rhs.bufferPtr;
            this->size_ = rhs.size_;
            this->capacity_ = rhs.capacity_;
            rhs.resetToSmall();
            return *this;
        }

        // If we already have sufficient space, assign the common elements, then
        // destroy any excess.
        SmallVectorSize rhsSize = rhs.size();
        SmallVectorSize curSize = this->size();
        if (curSize >= rhsSize) {
            // Assign common elements.
            iterator newEnd = this->begin();
            if (rhsSize)
            newEnd = std::move(rhs.begin(), rhs.end(), newEnd);

            // Destroy excess elements and trim the bounds.
            this->destroyRange(newEnd, this->end());
            this->setSize(rhsSize);

            // Clear the rhs.
            rhs.clear();

            return *this;
        }

        // If we have to grow to have enough elements, destroy the current elements.
        // This allows us to avoid copying them during the grow.
        // FIXME: this may not actually make any sense if we can efficiently move
        // elements.
        if (this->capacity() < rhsSize) {
            // Destroy current elements.
            this->destroyRange(this->begin(), this->end());
            this->setSize(0);
            curSize = 0;
            this->grow(rhsSize);
        } else if (curSize) {
            // Otherwise, use assignment for the already-constructed elements.
            std::move(rhs.begin(), rhs.begin() + curSize, this->begin());
        }

        // Move-construct the new elements in place.
        std::uninitialized_move(rhs.begin() + curSize, rhs.end(), this->begin() + curSize);

        // Set end.
        this->setSize(rhsSize);

        rhs.clear();
        return *this;
    }

    template <typename T, SmallVectorSize N>
    struct SmallVectorStorage {
        AlignedBuffer<T> inlineBuffer[N];
    };

    template <typename T> struct alignas(alignof(T)) SmallVectorStorage<T, 0> {};

    // Owns the inline buffer.
    template <class T, int N>
    class SmallVector : public SmallVectorImpl<T>, SmallVectorStorage<T, N> {
    public:
        SmallVector() : SmallVectorImpl<T>(N) {}

        SmallVector(SmallVectorSize size, const T &value = T()) : SmallVectorImpl<T>(N) {
            this->assign(size, value);
        }

        SmallVector(std::initializer_list<T> il) : SmallVectorImpl<T>(N) {
            this->assign(il);
        }

        SmallVector(const SmallVector &rhs) : SmallVectorImpl<T>(N) {
            if (!rhs.empty()) {
                SmallVectorImpl<T>::operator=(rhs);
            }
        }

        const SmallVector &operator=(const SmallVector &rhs) {
            SmallVectorImpl<T>::operator=(rhs);
            return *this;
        }

        SmallVector(SmallVector &&rhs) : SmallVectorImpl<T>(N) {
            if (!rhs.empty()) {
                SmallVectorImpl<T>::operator=(std::move(rhs));
            }
        }

        SmallVector(const SmallVectorImpl<T> &rhs) : SmallVectorImpl<T>(N) {
            if (!rhs.empty()) {
                SmallVectorImpl<T>::operator=(rhs);
            }
        }

        SmallVector(SmallVectorImpl<T> &&rhs) : SmallVectorImpl<T>(N) {
            if (!rhs.empty()) {
                SmallVectorImpl<T>::operator=(std::move(rhs));
            }
        }

        const SmallVector &operator=(SmallVector &&rhs) {
            SmallVectorImpl<T>::operator=(std::move(rhs));
            return *this;
        }

        const SmallVector &operator=(SmallVectorImpl<T> &&rhs) {
            SmallVectorImpl<T>::operator=(std::move(rhs));
            return *this;
        }

        const SmallVector &operator=(std::initializer_list<T> il) {
            this->assign(il);
            return *this;
        }

        ~SmallVector() {
            this->destroyRange(this->begin(), this->end());
        }

        T* inlineBufferPtr() noexcept { return SmallVectorStorage<T, N>::inlineBuffer[0].get(); }
    };

}
