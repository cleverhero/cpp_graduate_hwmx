set(MATRIX_HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../hwmx_lib/)

add_library(electrical_circuit_lib electrical_circuit.cpp electrical_circuit.h)
target_include_directories(electrical_circuit_lib PUBLIC ${MATRIX_HEADERS_DIR})
target_link_libraries(electrical_circuit_lib hwmx_lib)
