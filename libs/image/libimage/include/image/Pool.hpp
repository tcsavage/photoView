#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <memory>

namespace image {

    namespace detail {

        /**
         * @brief Maintains a ref count for a pooled resource.
         */
        struct PoolControl {
            int refCount { 0 };

            /**
             * @brief Returns true if the resource is not in use.
             */
            inline bool isAvailable() const noexcept { return refCount <= 0; }

            /**
             * @brief Increment refCount.
             */
            inline void retain() noexcept { ++refCount; }

            /**
             * @brief Decrement refCount and return true if refCount is now 0.
             */
            inline bool release() noexcept { return --refCount <= 0; }
        };

    }

    /**
     * @brief Carries a pointer to a pooled resource as well as a reference count.
     * 
     * When all leases for a pooled resource are destroyed, it becomes available for re-use.
     */
    template <class T>
    class PoolLease {
    public:
        T *operator->() { return ptr; }
        T &operator*() { return *ptr; }

        PoolLease(T *ptr, detail::PoolControl *ctrl) noexcept : ptr(ptr), ctrl(ctrl) { ctrl->retain(); }

        PoolLease(const PoolLease &other) noexcept : ptr(other.ptr), ctrl(other.ctrl) { ctrl->retain(); }
        PoolLease(PoolLease &&other) noexcept
          : ptr(std::exchange(other.ptr, nullptr))
          , ctrl(std::exchange(other.ctrl, nullptr)) {}

        PoolLease &operator=(const PoolLease &other) noexcept {
            return *this = PoolLease(other);
        }

        PoolLease &operator=(PoolLease &&other) noexcept {
            std::swap(ptr, other.ptr);
            std::swap(ctrl, other.ctrl);
            return *this;
        }

        ~PoolLease() noexcept { if (ctrl) { ctrl->release(); } }

    private:
        T *ptr { nullptr };
        detail::PoolControl *ctrl { nullptr };
    };

    template <class T>
    class PoolTraits {
        static T construct() noexcept { return T {}; }
        static void recycle(T &) noexcept { }
    };

    template <class T>
    class AbstractPool {
    public:
        virtual PoolLease<T> acquire() noexcept = 0;

        virtual ~AbstractPool() noexcept {}
    };

    /**
     * @brief Maintains a pool of up-to N resources of type T.
     * 
     * Resources are constructed lazily using PoolTraits<T>::construct, and recycled using PoolTraits<T>::recycle.
     * 
     * Calling acquire() provides access to the first available resource through a PoolLease<T> object. An acquired
     * resource is returned to the pool to be re-used once all outstanding PoolLease<T> objects are destroyed.
     */
    template <class T, std::size_t N>
    class Pool : public AbstractPool<T> {
    public:
        /**
         * @brief Returns a PoolLease<T> for the first available resource.
         */
        virtual PoolLease<T> acquire() noexcept override {
            auto it = std::find_if(control.begin(), control.end(), [](auto &ctrl) { return ctrl.isAvailable(); });
            if (it == control.end()) {
                // No resources available.
                // TODO: handle this condition more gracefully.
                std::terminate();
            }
            auto idx = it - control.begin();

            auto &slot = resources.at(idx);
            if (slot == nullptr) {
                slot = std::make_unique<T>(factory());
            } else {
                PoolTraits<T>::recycle(*slot);
            }
            return PoolLease { slot.get(), &*it };
        }

        /**
         * @brief Creates a new Pool. Any specified arguments will be forwarded to PoolTraits<T>::construct().
         */
        template <class ... Args>
        Pool(Args&&... args) noexcept
          : factory([... args = std::forward<Args>(args)] { return PoolTraits<T>::construct(args...); }) {}

        virtual ~Pool() noexcept {}

    private:
        std::array<detail::PoolControl, N> control;
        std::array<std::unique_ptr<T>, N> resources;
        std::function<T()> factory;
    };

}
