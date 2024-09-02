cmake_minimum_required(VERSION 2.8)
enable_testing()
include_directories(./)
include_directories(./tests/)
# set(CMAKE_C_STANDARD 99)
# set(CMAKE_C_STANDARD_REQUIRED ON) c3.10
# 设置调试构建类型
set(CMAKE_BUILD_TYPE Debug)

# 添加调试标志
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

add_definitions(-std=c99)

file(GLOB SOURCES "*.c")
function(add_my_executable exe_name)
    add_executable(${exe_name} ${SOURCES} ${ARGN})
    target_link_libraries(${exe_name} correct m)
endfunction(add_my_executable exe_name)

add_my_executable(test_qzss "tests/test_qzss.c")
add_my_executable(test_kpl "tests/test_kpl.c")
add_my_executable(test_b2b "tests/test_b2b.c")

