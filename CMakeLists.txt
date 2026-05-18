cmake_minimum_required(VERSION 4.1)
project(json_parser)

set(CMAKE_CXX_STANDARD 20)

add_executable(json_parser main.cpp
        jsonparser.cpp
        jsonparser.h
)
