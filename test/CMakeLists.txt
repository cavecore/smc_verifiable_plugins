find_package(GTest REQUIRED)

set(PLUGINS_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Unit tests
add_executable(test_plugin_loading ${CMAKE_CURRENT_SOURCE_DIR}/test_plugin_loading.cpp)
target_link_libraries(test_plugin_loading ${PROJECT_NAME} GTest::gtest GTest::gtest_main)
gtest_add_tests(TARGET test_plugin_loading WORKING_DIRECTORY ${PLUGINS_DIR})
