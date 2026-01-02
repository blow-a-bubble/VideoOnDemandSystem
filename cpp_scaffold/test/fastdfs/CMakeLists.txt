cmake_minimum_required(VERSION 3.1)
project(fastdfs)
set(CMAKE_CXX_STANDARD 17)
add_executable(test test.cpp ../../source/fastdfs.cc ../../source/logger.cc)
target_link_libraries(test PRIVATE
    fdfsclient fastcommon spdlog fmt
)