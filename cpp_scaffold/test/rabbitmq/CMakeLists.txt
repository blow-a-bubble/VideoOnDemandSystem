cmake_minimum_required(VERSION 3.10)
project(rabbitmq)
set(CMAKE_CXX_STANDARD 17)

add_executable(publish publish.cc ../../source/rabbitmq.cc ../../source/logger.cc)
target_link_libraries(publish PRIVATE
    amqpcpp ev spdlog fmt
)

add_executable(consume consume.cc ../../source/rabbitmq.cc ../../source/logger.cc)
target_link_libraries(consume PRIVATE
    amqpcpp ev spdlog fmt
)

add_executable(dlx_publish dlx_publish.cc ../../source/rabbitmq.cc ../../source/logger.cc)
target_link_libraries(dlx_publish PRIVATE
    amqpcpp ev spdlog fmt
)   
add_executable(dlx_consume dlx_consume.cc ../../source/rabbitmq.cc ../../source/logger.cc)
target_link_libraries(dlx_consume PRIVATE
    amqpcpp ev spdlog fmt
)
