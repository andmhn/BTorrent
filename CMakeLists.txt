cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(bittorent-client LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(bt-core)

add_subdirectory(bt-ui)

add_subdirectory(tests)
