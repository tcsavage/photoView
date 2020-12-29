cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(
    photo_luts
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "Apply LUTs to photographs"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

add_subdirectory(external)
add_subdirectory(libs)
add_subdirectory(app)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    include(GNUInstallDirs)
    install(FILES "packaging/xdg/dev.tcsavage.PhotoLuts.desktop" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications/")
    install(FILES "packaging/icons/dev.tcsavage.PhotoLuts.png" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps/")
endif()
