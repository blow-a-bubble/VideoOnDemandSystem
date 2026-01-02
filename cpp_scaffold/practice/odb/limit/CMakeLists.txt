cmake_minimum_required(VERSION 3.10)
project(TEST)
set(CMAKE_CXX_STANDARD 17)

# 查找ODB编译器
find_program(ODB_COMPILER odb)
if(NOT ODB_COMPILER)
    message(FATAL_ERROR "ODB compiler not found! Please install ODB.")
endif()

# 设置ODB编译选项
set(ODB_FLAGS 
    --database mysql
    --generate-query
    --generate-schema
    --profile boost/date-time
    --std c++11
    --output-dir ${CMAKE_CURRENT_BINARY_DIR}
)

# 头文件
set(STUDENT_HEADER student.hxx)

# 生成ODB源文件的自定义命令
add_custom_command(
    OUTPUT 
        student-odb.cxx
        student-odb.hxx
        student-odb.ixx
    COMMAND ${ODB_COMPILER} ${ODB_FLAGS} ${CMAKE_CURRENT_SOURCE_DIR}/${STUDENT_HEADER}
    DEPENDS ${STUDENT_HEADER}
    COMMENT "Generating ODB files from ${STUDENT_HEADER}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# 主可执行文件
add_executable(TEST main.cpp student-odb.cxx)

# 确保ODB文件在编译前生成
set_source_files_properties(student-odb.cxx PROPERTIES GENERATED TRUE)

target_link_libraries(TEST PRIVATE 
    odb-mysql 
    odb 
    odb-boost 
    pthread
    
)

# 添加包含目录
target_include_directories(TEST PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(TEST PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
