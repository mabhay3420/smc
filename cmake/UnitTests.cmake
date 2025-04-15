include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

set(COMMON_TEST_SRCS
    tests/test_main.cpp
)

set(lexer_TESTS_SRCS
    tests/lexer_test.cpp
    src/lexer.cpp
    ${COMMON_TEST_SRCS}
)

set(all_TEST_TARGETS
    lexer
)

foreach(test_target ${all_TEST_TARGETS})
    add_executable(
        ${test_target}_test
        ${${test_target}_TESTS_SRCS}
    )
    target_link_libraries(
        ${test_target}_test
        GTest::gtest_main
    )
    gtest_discover_tests(
        ${test_target}_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endforeach()
