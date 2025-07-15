cmake_minimum_required(VERSION 3.10)
project(Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server_prog
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/source/ExpressionManager.cpp
)

target_include_directories(server_prog PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)

FetchContent_MakeAvailable(googletest)
enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/tests)