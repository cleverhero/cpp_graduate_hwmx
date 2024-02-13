cmake_minimum_required(VERSION 3.16)
project(HWMX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MATRIX_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/hwmx_lib/)
set(GRAPH_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/graph_lib/)

enable_testing()

add_subdirectory(hwmx_lib)
add_subdirectory(graph_lib)
add_subdirectory(test)
add_subdirectory(benchmark)

add_executable(det src/det.cpp)
target_include_directories(det PUBLIC ${MATRIX_HEADERS_DIR})
target_link_libraries(det hwmx_lib)

add_executable(optimal_order src/optimal_order.cpp)
target_include_directories(optimal_order PUBLIC ${MATRIX_HEADERS_DIR})
target_link_libraries(optimal_order hwmx_lib)

add_executable(intensity src/intensity.cpp)
target_include_directories(intensity PUBLIC ${MATRIX_HEADERS_DIR} ${GRAPH_HEADERS_DIR})
target_link_libraries(intensity hwmx_lib)
target_link_libraries(intensity graph_lib)