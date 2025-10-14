cmake_minimum_required(VERSION 3.10)
project(etcd_test)

set(CMAKE_CXX_STANDARD 17)
set(PROTOC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/)
set(PROTOC_FILES cal.proto)
set(PROTOC_SRCS "")
foreach(protoc_file ${PROTOC_FILES})
    string(REPLACE "proto" "pb.cc" protoc_cxx ${protoc_file})
    string(REPLACE ".hxx" "pb.h" protoc_hxx ${protoc_file})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${protoc_cxx} ${CMAKE_CURRENT_BINARY_DIR}/${protoc_hxx}
        COMMAND protoc 
        ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR} -I${PROTOC_PATH} --experimental_allow_proto3_optional ${protoc_file} 
        DEPENDS ${PROTOC_PATH}/${protoc_file}
        COMMENT "生成protoc框架文件 for ${protoc_file}"
    )
    list(APPEND PROTOC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${protoc_cxx})
endforeach()




add_executable( discovery_test
    ${PROTOC_SRCS}
    discovery.cc
    ../../source/etcd.cc
    ../../source/logger.cc
    ../../source/util.cc
    ../../source/rpc.cc
)
target_include_directories(discovery_test PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR} 
)

target_link_libraries( discovery_test PRIVATE
    cpprest 
    etcd-cpp-api
    spdlog
    fmt
    jsoncpp
    brpc
    leveldb
    gflags
    ssl
    crypto 
    protobuf
    pthread
)

add_executable(registry_test
    ${PROTOC_SRCS}
    registry.cc
    ../../source/etcd.cc
    ../../source/logger.cc
    ../../source/util.cc
    ../../source/rpc.cc
)

target_include_directories(registry_test PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR} 
)

target_link_libraries( registry_test PRIVATE
    cpprest 
    etcd-cpp-api
    spdlog
    fmt
    jsoncpp
    brpc
    leveldb
    gflags
    ssl
    crypto 
    protobuf
    pthread
)








