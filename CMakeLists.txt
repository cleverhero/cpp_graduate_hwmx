cmake_minimum_required(VERSION 3.16)
project(HWMX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)

add_executable(det src/det.cpp)
target_link_libraries(det hwmx_lib)
