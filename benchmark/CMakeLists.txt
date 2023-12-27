set(MATRIX_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../hwmx_lib/)


add_executable(base_vs_lazy base_vs_lazy.cpp)
target_include_directories(base_vs_lazy PUBLIC ${MATRIX_HEADERS_DIR})
target_link_libraries(base_vs_lazy hwmx_lib)


add_executable(matrix_chain_bench matrix_chain.cpp)
target_include_directories(matrix_chain_bench PUBLIC ${MATRIX_HEADERS_DIR})
target_link_libraries(matrix_chain_bench hwmx_lib)