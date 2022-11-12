cmake_minimum_required(VERSION 3.12)
project(RageVision)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

include_directories("include")
file(GLOB SOURCES "src/*.cxx")
add_executable(RageVision ${SOURCES})
target_link_libraries(RageVision opencv_core)
