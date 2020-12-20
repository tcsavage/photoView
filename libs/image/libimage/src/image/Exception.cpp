#include <image/Exception.hpp>

namespace image {

    Exception::Exception(std::string msg)
        : _msg(msg) {}

    const char* Exception::what() const noexcept {
        return _msg.c_str();
    }

}
