cmake_minimum_required(VERSION 3.10)

project(Memento VERSION 0.0.1)
add_executable(Memento ${PROJECT_SOURCE_DIR}/src/main.cpp)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)