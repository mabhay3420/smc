cmake_minimum_required(VERSION 3.25)
project(smc VERSION 0.0.1)
enable_testing()
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

add_executable(
    smc
    src/main.cpp
    src/lexer.cpp
)

include_directories(include)

# Add extra scripts
include(cmake/Extras.cmake)


# Testing
include(cmake/UnitTests.cmake)