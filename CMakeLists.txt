cmake_minimum_required(VERSION 3.24)

project(manticore
    VERSION 1.0.0
    DESCRIPTION "Main thread function executor"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(manticore INTERFACE)
add_library(tatami::manticore ALIAS manticore)

# Switch between include directories depending on whether the downstream is
# using the build directly or is using the installed package.
include(GNUInstallDirs)
target_include_directories(manticore
    INTERFACE 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/tatami_manticore>"
)

# Installing for find_package.
include(CMakePackageConfigHelpers)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tatami_manticore)

install(TARGETS manticore
    EXPORT manticoreTargets)

install(EXPORT manticoreTargets
    FILE tatami_manticoreTargets.cmake
    NAMESPACE tatami::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tatami_manticore)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/tatami_manticoreConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tatami_manticore)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tatami_manticoreConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tatami_manticore)
