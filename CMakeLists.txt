cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(bittorent-client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(bt-core)

# Building UI is resource hungry so we skip it while testing
if(NOT TEST_ONLY)
    add_subdirectory(bt-ui)
endif()

add_subdirectory(tests)
