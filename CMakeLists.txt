cmake_minimum_required(VERSION 3.10)
project(search_engine_v0)

add_executable(search_engine_v0 engine/src/main.cpp
    engine/src/tokenizer.cpp
    engine/src/stemmer.cpp
    engine/src/text_processor.cpp
    engine/src/stopword_filter.cpp)


find_package(Catch2 3 REQUIRED)
add_executable(search_engine_tests
    tests/test_main.cpp
    engine/src/tokenizer.cpp
    engine/src/stemmer.cpp
    engine/src/text_processor.cpp
    engine/src/stopword_filter.cpp)

find_path(SNOWBALL_INCLUDE_DIR
    NAME libstemmer.h
    PATHS /opt/homebrew/opt/snowball/lib
)

find_library(SNOWBALL_LIBRARY
    NAMES stemmer
    PATHS /opt/homebrew/opt/snowball/lib
)

if(NOT SNOWBALL_INCLUDE_DIR OR NOT SNOWBALL_LIBRARY)
    message(FATAL_ERROR "Snowball library not found")
endif()

target_link_libraries(search_engine_tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(search_engine_tests PRIVATE engine/include)

target_include_directories(search_engine_v0 PRIVATE ${SNOWBALL_INCLUDE_DIR})
target_link_libraries(search_engine_v0 PRIVATE ${SNOWBALL_LIBRARY})

target_include_directories(search_engine_tests PRIVATE engine/include)
target_link_libraries(search_engine_tests PRIVATE ${SNOWBALL_LIBRARY})

include(CTest)
include(Catch)
catch_discover_tests(
    search_engine_tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
