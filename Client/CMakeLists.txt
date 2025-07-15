cmake_minimum_required(VERSION 3.10)
project(Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(client_prog
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/source/ExpressionGenerator.cpp
    ${CMAKE_SOURCE_DIR}/include/utils.h

)

target_include_directories(client_prog PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)