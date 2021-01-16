#pragma once

#include <iostream>

#include <image/TypeName.hpp>

namespace image::opencl {

    /**
     * @brief Class for wrapping OpecCL resource management.
     * 
     * @tparam T OpenCL resource handle type (e.g. cl_context)
     * @tparam Retain Pointer to retain function (e.g. clRetainContext)
     * @tparam Release Pointer to release function (e.g. clReleaseContext)
     */
    template <class T, auto Retain, auto Release>
    class Handle {
    public:
        /**
         * @brief Returns the underlying handle value.
         */
        constexpr T get() const noexcept { return handle; }

        void incRef() const noexcept {
            if (handle) { Retain(handle); }
        }

        void decRef() const noexcept {
            if (handle) { Release(handle); }
        }

        /**
         * @brief Create a Handle<> which assumes ownership of the resource.
         * 
         * The function simply wraps the raw handle value without calling retain.
         */
        static Handle<T, Retain, Release> takeOwnership(const T &rawHandle) noexcept {
            Handle<T, Retain, Release> out { rawHandle };
            return out;
        }

        /**
         * @brief Creates a Handle<> which retains an already managed resource.
         * 
         * This function is like takeOwnership except that it also calls retain.
         */
        static Handle<T, Retain, Release> retain(const T &rawHandle) noexcept {
            Handle<T, Retain, Release> out { rawHandle };
            out.incRef();
            return out;
        }

        Handle() noexcept {}

        Handle(const Handle &other) noexcept : handle(other.handle) {
            std::cerr << "Retaining " << TypeName<T>::get() << " resource (copy constructor)\n";
            Retain(handle);
        }

        Handle(Handle &&other) noexcept : handle(other.handle) { other.handle = 0; }

        Handle &operator=(const Handle &rhs) noexcept {
            handle = rhs.handle;
            std::cerr << "Retaining " << TypeName<T>::get() << " resource (copy assignment)\n";
            Retain(handle);
            return *this;
        }

        Handle &operator=(Handle &&rhs) noexcept {
            std::swap(handle, rhs.handle);
            return *this;
        }

        ~Handle() noexcept {
            if (handle) {
                std::cerr << "Releasing " << TypeName<T>::get() << " resource\n";
                Release(handle);
                handle = 0;
            }
        }
    
    protected:
        explicit Handle(const T &handle) noexcept : handle(handle) {
            std::cerr << "Acquired new " << TypeName<T>::get() << " resource\n";
        }

    private:
        T handle { 0 };
    };

}
