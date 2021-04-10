#include <image/Assert.hpp>

#include <csignal>
#include <iostream>

#ifdef __cpp_lib_source_location

bool logAssertionError(const std::string_view expr,
                       const std::string_view message,
                       const std::source_location &location) noexcept {
    std::cerr << "Assertion '" << expr << "' failed.\n"
              << "  in file " << location.file_name() << ":" << location.line() << ":" << location.column() << "\n"
              << "  function: " << location.function_name() << "\n"
              << "  with message: " << message << "\n";
    std::raise(SIGINT);
    return true;
}

#else

bool logAssertionError(const std::string_view expr,
                       const std::string_view message,
                       const std::string_view file,
                       const std::string_view function,
                       int line) noexcept {
    std::cerr << "\nAssertion '" << expr << "' failed.\n"
              << "  in file " << file << ":" << line << "\n"
              << "  function: " << function << "\n"
              << "  with message: " << message << "\n";
    std::raise(SIGINT);
    return true;
}

#endif
