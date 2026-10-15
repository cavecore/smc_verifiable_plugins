find_package(GTest REQUIRED)

set(PLUGINS_DIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# One gtest executable per test_*.cpp file found, named after the file.
file(GLOB TEST_SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp)

foreach(test_source ${TEST_SOURCE_FILES})
  get_filename_component(test_name ${test_source} NAME_WE)
  add_executable(${test_name} ${test_source})
  target_link_libraries(${test_name} ${PROJECT_NAME} GTest::gtest GTest::gtest_main)
  gtest_add_tests(TARGET ${test_name} WORKING_DIRECTORY ${PLUGINS_DIR})
endforeach()
