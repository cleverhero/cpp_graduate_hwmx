set(
    HEADERS_LIST 
    elements_buffer.h
    iterators.h
    matrix_line.h
    vector.h
    matrix.h
)

add_library(hwmx_lib hwmx.cpp ${HEADERS_LIST})
