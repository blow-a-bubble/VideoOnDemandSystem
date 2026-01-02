cmake_minimum_required(VERSION 3.10)
project(rabbitmq)
set(CMAKE_CXX_STANDARD 17)

add_executable(publish publish.cpp)
target_link_libraries(publish PRIVATE
    amqpcpp ev
)

add_executable(consume consume.cpp)
target_link_libraries(consume PRIVATE
    amqpcpp ev
)

# 延时队列
add_executable(dlx_publish dlx_publish.cpp)
target_link_libraries(dlx_publish PRIVATE
    amqpcpp ev
)   
add_executable(dlx_consume dlx_consume.cpp)
target_link_libraries(dlx_consume PRIVATE   
    amqpcpp ev
)  

