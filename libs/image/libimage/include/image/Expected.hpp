#pragma once

#include <cassert>
#include <exception>
#include <optional>

namespace image {

    // Wrapper for the unexpected branch.
    template <class E>
    class Unexpected {
    public:
        constexpr E value() const noexcept { return value_; }

        Unexpected(E value) : value_(value) {}

    private:
        E value_;
    };


    // Trivial value used by Expected<void, E> to indicate success.
    class Success {};
    inline Success success;


    template <class T, class E>
    class Expected {
    public:
        constexpr T& operator*() {
            assert(isValue_);
            return value_;
        }

        constexpr const T& operator*() const {
            assert(isValue_);
            return value_;
        }

        constexpr T* operator->() { return &**this; }
        constexpr const T* operator->() const { return &**this; }

        constexpr E& error() const {
            assert(!isValue_);
            return error_;
        }

        constexpr bool hasValue() const noexcept { return isValue_; }
        constexpr bool hasError() const noexcept { return !isValue_; }

        explicit operator bool() const noexcept { return isValue_; }

        constexpr T &value() {
            if (!isValue_) {
                throw error_;
            }
            return value_;
        }

        constexpr const T &value() const {
            if (!isValue_) {
                throw error_;
            }
            return value_;
        }

        template <class U>
        constexpr T valueOr(U&& alt) const& {
            if (!isValue_) {
                return T(std::forward<U>(alt));
            } else {
                return value_;
            }
        }

        template <class U>
        constexpr T valueOr(U&& alt) && {
            if (!isValue_) {
                return T(std::forward<U>(alt));
            } else {
                return std::move(value_);
            }
        }

        constexpr Expected() noexcept { new (&value_) T(); }
        constexpr Expected(const T& rhs) noexcept { new (&value_) T(rhs); }
        constexpr Expected(const Unexpected<E>& rhs) noexcept : isValue_(false) { new (&error_) E(rhs.value()); }
        template <class U = T>
        constexpr explicit Expected(U&& rhs) noexcept { new (&value_) T(std::forward<U>(rhs)); }

        constexpr Expected(const Expected& rhs) noexcept : isValue_(rhs.isValue_) {
            if (isValue_) {
                new (&value_) T(rhs.value_);
            } else {
                new (&error_) E(rhs.error_);
            }
        }

        constexpr Expected(Expected&& rhs) noexcept : isValue_(rhs.isValue_) {
            if (isValue_) {
                new (&value_) T(std::move(rhs.value_));
            } else {
                new (&error_) E(std::move(rhs.error_));
            }
        }

    private:
        union {
            T value_;
            std::exception_ptr error_;
        };
        bool isValue_ { true };
    };


    template <class E>
    class Expected<void, E> {
    public:
        constexpr E& error() const {
            assert(!error_.has_value());
            return error_;
        }
        bool hasError() const noexcept { return error_.has_value(); }

        explicit operator bool() const noexcept { return !error_.has_value(); }

        constexpr Expected() noexcept : error_(std::nullopt) {}
        constexpr Expected(Success) : Expected() {}
        constexpr Expected(const Unexpected<E>& rhs) noexcept : error_(rhs.value()) {}

        constexpr Expected(const Expected& rhs) noexcept : error_(rhs.error_) {}

        constexpr Expected(Expected&& rhs) noexcept : error_(std::move(rhs.error_)) {}

    private:
        std::optional<E> error_;
    };

}
