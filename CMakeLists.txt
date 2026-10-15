cmake_minimum_required(VERSION 3.7)

# set the project name and version
project(smc_verifiable_plugins VERSION 1.0)

# Use RelWithDebInfo by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()
# Enable compile commands export for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# Required for perf
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
# Additional flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3")

# Make sure lib and binaries are separated
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Install it only if strictly required
option(SMC_PLUGINS_EXPORT_LIB "Whether to install export the library in the cmake registry." ON)
if(SMC_PLUGINS_EXPORT_LIB AND NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(SMC_PLUGINS_EXPORT_LIB OFF)
endif()

# The library, composed by header files only
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

# Generate the various plugins
set(PLUGIN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/plugins/include)

add_library(int_accumulation_smc_plugin SHARED ${CMAKE_CURRENT_SOURCE_DIR}/plugins/src/int_accumulation_smc_plugin.cpp)
target_link_libraries(int_accumulation_smc_plugin ${PROJECT_NAME})
target_include_directories(int_accumulation_smc_plugin PUBLIC ${PLUGIN_INCLUDE_DIR})


include(CTest)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()


### Install the package: required only in case we are building it manually
if(SMC_PLUGINS_EXPORT_LIB)
  # Copy the include files over
  add_custom_target(${PROJECT_NAME}_includes
    ALL ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}"
      "${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}"
    COMMENT "Copying include folder."
  )

  # Generate the cmake modules
  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
      "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
      VERSION ${PROJECT_VERSION}
      COMPATIBILITY AnyNewerVersion
  )
  configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
  )

  # Export everything
  install(TARGETS ${PROJECT_NAME}
      EXPORT ${PROJECT_NAME}Targets
      LIBRARY DESTINATION lib COMPONENT Runtime
      ARCHIVE DESTINATION lib COMPONENT Development
      RUNTIME DESTINATION bin COMPONENT Runtime
      PUBLIC_HEADER DESTINATION include COMPONENT Development
      BUNDLE DESTINATION bin COMPONENT Runtime
  )

  export(EXPORT ${PROJECT_NAME}Targets
        FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
        NAMESPACE ${PROJECT_NAME}::
  )

  export(PACKAGE ${PROJECT_NAME})
endif()
