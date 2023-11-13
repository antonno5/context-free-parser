cmake_minimum_required(VERSION 3.26)
project(context_free_parser)

set(CMAKE_CXX_STANDARD 20)

add_executable(context_free_parser
        src/main.cpp
        src/grammar.h src/grammar.cpp
        src/Earley.h src/Earley.cpp)
