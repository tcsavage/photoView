cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(
    photoView
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "Photograph viewer with 3D LUT support"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

add_subdirectory(external)
add_subdirectory(libs)
add_subdirectory(app)
add_subdirectory(v2_test)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    include(GNUInstallDirs)
    install(FILES "packaging/xdg/dev.tomsavage.PhotoView.desktop" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications/")
    install(FILES "packaging/icons/dev.tomsavage.PhotoView.svg" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps/")
endif()
