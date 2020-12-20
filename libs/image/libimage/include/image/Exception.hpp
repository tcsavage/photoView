#pragma once

#include <image/CoreTypes.hpp>

namespace image {

    class Exception : public std::exception {
    public:
        Exception(String msg);

        virtual const char* what() const noexcept override;

    private:
        String _msg;
    };

    class OutOfBoundsException : public Exception {
    public:
        OutOfBoundsException()
            : Exception("Out of bounds") {}

        OutOfBoundsException(String msg)
            : Exception("Out of bounds: " + msg) {}
    };

    class IOException : public Exception {
    public:
        IOException()
            : Exception("IO Exception") {}

        IOException(String msg)
            : Exception("IO Exception: " + msg) {}
    };

    class TypeMismatchException : public Exception {
    public:
        TypeMismatchException()
            : Exception("Type missmatch") {}

        TypeMismatchException(String msg)
            : Exception("Type missmatch: " + msg) {}
    };

}
