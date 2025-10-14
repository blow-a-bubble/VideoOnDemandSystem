cmake_minimum_required(VERSION 3.10)
project(example)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/source)
add_executable(test test.cpp)