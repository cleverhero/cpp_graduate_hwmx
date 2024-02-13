set(MATRIX_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../hwmx_lib/)

add_library(graph_lib graph.cpp graph.h)
target_include_directories(graph_lib PUBLIC ${MATRIX_HEADERS_DIR})
target_link_libraries(graph_lib hwmx_lib)
