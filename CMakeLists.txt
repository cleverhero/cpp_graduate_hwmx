cmake_minimum_required(VERSION 3.16)
project(HWMX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(benchmark)

add_executable(det src/det.cpp)
target_link_libraries(det hwmx_lib)

add_executable(optimal_order src/optimal_order.cpp)
target_link_libraries(optimal_order hwmx_lib)