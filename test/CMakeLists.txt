include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)

set(RESOURCES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../resources/")
get_filename_component(RESOURCES_PATH ${RESOURCES_PATH} ABSOLUTE)
configure_file(config.h.in config.h @ONLY)

set(CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src/)

add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(es_test exception_safety_test.cpp)
target_include_directories(es_test PUBLIC ${HEADERS_DIR})
target_link_libraries(es_test PRIVATE GTest::GTest hwmx_lib)

add_executable(det_test det_test.cpp)
target_include_directories(det_test PUBLIC ${CONFIG_DIR} ${HEADERS_DIR})
target_link_libraries(det_test PRIVATE GTest::GTest hwmx_lib)

add_executable(lazy_behavior_test lazy_behavior_test.cpp)
target_include_directories(lazy_behavior_test PUBLIC ${CONFIG_DIR} ${HEADERS_DIR})
target_link_libraries(lazy_behavior_test PRIVATE GTest::GTest hwmx_lib)


add_test(es_test es_test)
add_test(det_test det_test)
add_test(lazy_behavior_test lazy_behavior_test)
