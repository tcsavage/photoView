#include <image/Exception.hpp>

namespace image {

    Exception::Exception(std::string msg)
        : msg_(msg) {}

    const char* Exception::what() const noexcept {
        return msg_.c_str();
    }

}
