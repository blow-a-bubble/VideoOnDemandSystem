cmake_minimum_required(VERSION 3.2)
project(odb_test)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(ODB_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(ODB_FILES student.hxx)
set(ODB_SRCS "")
foreach(odb_file ${ODB_FILES})
    string(REPLACE ".hxx" "-odb.cxx" odb_cxx ${odb_file})
    string(REPLACE ".hxx" "-odb.hxx" odb_hxx ${odb_file})
    if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/${odb_cxx})
        message(STATUS "Generating ODB files for ${odb_file}")
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${odb_cxx} ${CMAKE_CURRENT_BINARY_DIR}/${odb_hxx}
            COMMAND odb 
            ARGS -d mysql  --generate-query --generate-schema --profile boost/date-time ${ODB_PATH}/${odb_file}
            DEPENDS ${ODB_PATH}/${odb_file}
            COMMENT "生成odb框架文件 for ${file}"
        )
    endif()
    list(APPEND ODB_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${odb_cxx})
endforeach()
file(GLOB SRCS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
message(STATUS "ODB_SRCS: ${ODB_SRCS}")
add_executable(odb_test ${ODB_SRCS} ${SRCS_FILES})
target_include_directories(odb_test PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(odb_test odb odb-mysql odb-boost)
