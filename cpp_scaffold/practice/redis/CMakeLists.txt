cmake_minimum_required(VERSION 3.10)
project(RedisExample)
set(CMAKE_CXX_STANDARD 17)

add_executable(pipeline pipeline.cpp)

target_link_libraries(pipeline PRIVATE 
    -lhiredis -lredis++ -lpthread
)

add_executable(transaction transaction.cpp)
target_link_libraries(transaction PRIVATE 
    -lhiredis -lredis++ -lpthread
)
