cmake_minimum_required(VERSION 3.10)
project(fastdfs_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(upload upload.cpp)
target_link_libraries(upload PRIVATE
    -lfdfsclient -lfastcommon
)

add_custom_command(
    TARGET upload POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/client.conf
            ${CMAKE_SOURCE_DIR}/test_upload.txt
            ${CMAKE_BINARY_DIR}/
)


add_executable(download download.cpp)
target_link_libraries(download PRIVATE
    -lfdfsclient -lfastcommon
)


add_custom_command(
    TARGET download POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/client.conf
            ${CMAKE_BINARY_DIR}/
)


add_executable(remove remove.cpp)
target_link_libraries(remove PRIVATE
    -lfdfsclient -lfastcommon
)


add_custom_command(
    TARGET remove POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/client.conf
            ${CMAKE_BINARY_DIR}/
)