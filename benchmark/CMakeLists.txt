set(HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src/)


add_executable(base_vs_lazy base_vs_lazy.cpp)
target_include_directories(base_vs_lazy PUBLIC ${HEADERS_DIR})
target_link_libraries(base_vs_lazy hwmx_lib)