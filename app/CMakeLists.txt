set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets CONFIG REQUIRED)

set(photo_luts_sources
    resources.qrc
    src/main.cpp
    src/ImageView.cpp
    src/MainWindow.cpp
    src/Processor.cpp
    src/ProcessorController.cpp
    src/ProcessorWorker.cpp
)

add_executable(photo_luts ${photo_luts_sources})

target_link_libraries(
    photo_luts
    PRIVATE
    Qt5::Widgets
    image::libimage
)

target_compile_features(photo_luts PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(photo_luts PRIVATE /W4)
else()
    target_compile_options(photo_luts PRIVATE -Wall -Wextra -pedantic -Werror)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT photo_luts_DISABLE_ASAN)
        target_compile_options(photo_luts PRIVATE -fsanitize=address)
        target_link_libraries(photo_luts PRIVATE -fsanitize=address)
    endif()
endif()

include(GNUInstallDirs)
install(TARGETS photo_luts RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
