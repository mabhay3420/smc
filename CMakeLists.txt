cmake_minimum_required(VERSION 3.25)
project(smc)

add_executable(
    smc
    src/main.cpp
    src/lexer.cpp
)

include_directories(include)

# Add extra scripts
include(Extras.cmake)

