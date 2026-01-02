cmake_minimum_required(VERSION 3.10)
project(RedisExample)
set(CMAKE_CXX_STANDARD 17)
add_executable(test test.cpp ../../source/redis.cc ../../source/logger.cc)
target_link_libraries(test PRIVATE 
    hiredis redis++ pthread spdlog fmt
)

