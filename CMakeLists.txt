cmake_minimum_required(VERSION 3.12)
project(RageVision)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(apriltag REQUIRED apriltag)

include_directories("include" ${OpenCV_INCLUDE_DIRS} ${apriltag_INCLUDE_DIRS})
file(GLOB SOURCES "src/*.cxx")
add_executable(RageVision ${SOURCES})
target_link_libraries(RageVision ${OpenCV_LIBS} ${apriltag_LIBRARIES})
add_custom_target(Calibrate ALL COMMAND ${CMAKE_COMMAND} -E create_symlink RageVision Calibrate)
