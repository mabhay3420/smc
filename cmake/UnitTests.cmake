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
    src/utils.cpp
)

set(lexer_TESTS_SRCS
    tests/lexer_test.cpp
    src/lexer.cpp
    ${COMMON_TEST_SRCS}
)

set(parser_TESTS_SRCS
    tests/parser_test.cpp
    src/lexer.cpp
    src/parser.cpp
    ${COMMON_TEST_SRCS}
)
set(all_TEST_TARGETS
    lexer
    parser
)

set(all_TEST_TARGET_LIST)

message(STATUS "CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

foreach(test_target ${all_TEST_TARGETS})
    add_executable(
        ${test_target}_test
        ${${test_target}_TESTS_SRCS}
    )
    # list(APPEND all_TEST_TARGET_LIST ${CMAKE_BINARY_DIR}/${test_target}_test)
    list(APPEND all_TEST_TARGET_LIST ${test_target}_test)
    target_link_libraries(
        ${test_target}_test
        GTest::gtest_main
    )
    gtest_discover_tests(
        ${test_target}_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTIES
          ENVIRONMENT "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/${test_target}-%p.profraw"
      )
endforeach()

if(ENABLE_COVERAGE AND LLVM_PROFDATA AND LLVM_COV)
    message(STATUS "Enabling coverage reporting")
    message(STATUS "LLVM_PROFDATA: ${LLVM_PROFDATA}")
    message(STATUS "LLVM_COV: ${LLVM_COV}")
    add_custom_target(coverage
        COMMENT "→ running tests, merging .profraw, generating HTML with llvm-cov"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}

        # 1) clean out old raw data
        COMMAND ${CMAKE_COMMAND} -E remove -f *.profraw coverage.profdata

        # 2) run all tests (will produce .profraw next to each exe)
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure

        # 3) merge them
        COMMAND ${LLVM_PROFDATA}
        merge -sparse *.profraw
        -o coverage.profdata

        # 4) emit HTML for every test exe
        #    you can list them explicitly, or glob:
        COMMAND ${LLVM_COV}
        show
        ${all_TEST_TARGET_LIST}
        --format=html
        --instr-profile=coverage.profdata
        --output-dir=coverage-report

        # 5) open the report on macOS
        # COMMAND open coverage-report/index.html
    )
endif()
