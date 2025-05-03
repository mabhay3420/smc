cmake_minimum_required(VERSION 3.25)
project(smc VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "")

# Find LLVM
include(cmake/LLVM.cmake)

add_executable(
    smc
    src/main.cpp
    src/lexer.cpp
    src/utils.cpp
    src/parser.cpp
)

target_link_libraries(
    smc
    PRIVATE
    LLVMSupport
)

include_directories(include)

message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")

if(ENABLE_COVERAGE)
    message(STATUS "TESTING IF THIS ACTUALLY GETS SETUP")
    add_compile_options(-g -O0 -fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate -fcoverage-mapping)
endif()
message(STATUS "CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

enable_testing()

# Add extra scripts
include(cmake/Extras.cmake)


# Testing
include(cmake/UnitTests.cmake)
