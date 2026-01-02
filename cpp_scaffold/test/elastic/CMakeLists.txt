cmake_minimum_required(VERSION 3.10)
project(TEST)
set(CMAKE_CXX_STANDARD 17)

# 主可执行文件
# add_executable(TEST test_base.cc ../../source/elastic.cc ../../source/util.cc ../../source/logger.cc)

# target_link_libraries(TEST jsoncpp fmt spdlog elasticlient)



add_executable(ES es_test.cc ../../source/elastic.cc ../../source/util.cc ../../source/logger.cc)
target_link_libraries(ES jsoncpp fmt spdlog elasticlient)



