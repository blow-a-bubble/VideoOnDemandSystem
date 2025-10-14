cmake_minimum_required(VERSION 3.10)
project(etcd_test)

set(CMAKE_CXX_STANDARD 17)

add_executable( discovery_test
    discovery.cc
    ../../source/etcd.cc
    ../../source/logger.cc
    ../../source/util.cc
)

target_link_libraries( discovery_test PRIVATE
    cpprest 
    etcd-cpp-api
    spdlog
    fmt
    jsoncpp
)

add_executable( registry_test
    registry.cc
    ../../source/etcd.cc
    ../../source/logger.cc
    ../../source/util.cc
)

target_link_libraries( registry_test PRIVATE
    cpprest 
    etcd-cpp-api
    spdlog
    fmt
    jsoncpp
)
