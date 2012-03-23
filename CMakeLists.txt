cmake_minimum_required(VERSION 2.6)
project(g35d)

include_directories(include)
link_directories(../libs)

set(CMAKE_C_FLAGS "-g -Wall")

add_executable(g35d g35d.c g35config.c)
target_link_libraries(g35d g35 pthread confuse)
