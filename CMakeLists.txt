cmake_minimum_required(VERSION 3.10)
project(RayTracing C)

set(CMAKE_C_STANDARD 11)


include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/external/include)

add_definitions(-DPROJECT_DIR=\"${CMAKE_SOURCE_DIR}\")

add_subdirectory(src)

