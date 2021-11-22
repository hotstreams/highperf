cmake_minimum_required(VERSION 3.19)
project(lab1)

set(CMAKE_CXX_STANDARD 17)
add_compile_options("-fopenmp")
add_compile_options("-O3")
add_link_options("-fopenmp")

add_executable(lab1 main.cpp)