cmake_minimum_required(VERSION 3.10)
project(search_engine_v0)

add_executable(search_engine_v0 engine/src/main.cpp
    engine/src/tokenizer.cpp)


find_package(Catch2 3 REQUIRED)
add_executable(search_engine_tests
    tests/test_main.cpp
    engine/src/tokenizer.cpp
)

target_link_libraries(search_engine_tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(search_engine_tests PRIVATE engine/include)

include(CTest)
include(Catch)
catch_discover_tests(search_engine_tests)
