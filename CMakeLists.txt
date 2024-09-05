cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(bittorent-client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# configure exececutable directory
set(FullOutputDir "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${FullOutputDir}/static_libs")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${FullOutputDir}")

add_subdirectory(bt-core)

# Building UI is resource hungry so we skip it while testing
if(NOT TEST_ONLY)
    add_subdirectory(bt-ui)
endif()

add_subdirectory(tests)
