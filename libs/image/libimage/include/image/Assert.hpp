#pragma once

#include <string_view>

#ifdef __cpp_lib_source_location

#include <source_location>

bool logAssertionError(const std::string_view expr,
                       const std::string_view message,
                       const std::source_location &location) noexcept;

#ifdef NDEBUG
#define ASSERT(x, message) ((void)(0))
#else
#define ASSERT(x, message) ((void)(!(x) && logAssertionError(#x, message, std::source_location::current())))
#endif

#else

bool logAssertionError(const std::string_view expr,
                       const std::string_view message,
                       const std::string_view file,
                       const std::string_view function,
                       int line) noexcept;

#ifdef NDEBUG
#define ASSERT(x, message) ((void)(0))
#else
#define ASSERT(x, message) ((void)(!(x) && logAssertionError(#x, message, __FILE__, __FUNCTION__, __LINE__)))
#endif

#endif
