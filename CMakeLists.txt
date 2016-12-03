cmake_minimum_required(VERSION 3.6)
project(proj3)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -Werror")

set(SOURCE_FILES proj3.c)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(proj3 ${SOURCE_FILES})
