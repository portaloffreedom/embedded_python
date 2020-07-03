cmake_minimum_required(VERSION 3.17)
project(embedded_python)

set(CMAKE_CXX_STANDARD 17)

find_package(Python3 REQUIRED COMPONENTS Development)

add_executable(call
        main.cpp
        cppython/cppython.h
        cppython/CPPyObject.h)

target_link_libraries(call Python3::Python)
