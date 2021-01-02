set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets CONFIG REQUIRED)

set(photoView_sources
    resources.qrc
    src/main.cpp
    src/HaldImageDialog.cpp
    src/ImageView.cpp
    src/MainWindow.cpp
    src/OpenFileState.cpp
    src/ProcessingIndicator.cpp
    src/Processor.cpp
    src/ProcessorController.cpp
    src/ProcessorWorker.cpp
)

add_executable(photoView ${photoView_sources})

target_link_libraries(
    photoView
    PRIVATE
    Qt5::Widgets
    image::libimage
)

target_compile_features(photoView PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(photoView PRIVATE /W4)
else()
    target_compile_options(photoView PRIVATE -Wall -Wextra -pedantic -Werror)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT photoView_DISABLE_ASAN)
        target_compile_options(photoView PRIVATE -fsanitize=address)
        target_link_libraries(photoView PRIVATE -fsanitize=address)
    endif()
endif()

include(GNUInstallDirs)
install(TARGETS photoView RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
