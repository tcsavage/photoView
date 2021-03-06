add_executable(libimage_example_opencl main.cpp)
target_link_libraries(libimage_example_opencl PUBLIC image::libimage)

target_compile_features(libimage_example_opencl PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(libimage_example_opencl PRIVATE /W4 /WX)
else()
    target_compile_options(libimage_example_opencl PRIVATE -Wall -Wextra -pedantic -Werror -march=native)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT IMAGE_DISABLE_ASAN)
        target_compile_options(libimage_example_opencl PRIVATE -fsanitize=address)
        target_link_libraries(libimage_example_opencl PRIVATE -fsanitize=address)
    endif()
endif()

if(MSVC)
    target_compile_options(libimage_example_opencl PRIVATE /arch:AVX2)
else()
    target_compile_options(libimage_example_opencl PRIVATE -mavx2)
endif()

include(GNUInstallDirs)
install(TARGETS libimage_example_opencl RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
