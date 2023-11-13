cmake_minimum_required(VERSION 3.26)
project(context_free_parser)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(context_free_parser
        src/main.cpp
        src/grammar.h src/grammar.cpp
        src/Earley.h src/Earley.cpp)

enable_testing()

add_executable(
        parser_test
        src/tests/tests.cpp
        src/grammar.h src/grammar.cpp
        src/Earley.h src/Earley.cpp
)

target_link_libraries(
        parser_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(parser_test)
