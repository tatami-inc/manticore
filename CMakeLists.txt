cmake_minimum_required(VERSION 3.14)

project(manticore
    VERSION 1.0.0
    DESCRIPTION "Main thread function executor"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(manticore INTERFACE)

# Switch between include directories depending on whether the downstream is
# using the build directly or is using the installed package.
target_include_directories(manticore
    INTERFACE 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
