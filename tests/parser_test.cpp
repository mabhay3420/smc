#include "parser.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

//========================================================================
// Test Fixtures
//========================================================================

using TokenList = std::vector<lexer::Token>;
struct TestParseStateList : public ::testing::Test {

    std::vector<std::tuple<std::string, std::string, std::vector<std::string>>>
        testCases;

    TestParseStateList() {
        testCases = {{"[a], b, c", "a", {"a", "b", "c"}},
                     {"a, [b], c", "b", {"a", "b", "c"}},
                     {"a, b, [c]", "c", {"a", "b", "c"}}};
    }

  protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TestParseStateList, sample_test) {
    for (auto [src, intial_state, states] : testCases) {
        auto lexer = std::make_unique<lexer::Lexer>(src, false);
        auto parser = std::make_unique<parser::Parser>(std::move(lexer));
        parser->parse();
        ASSERT_EQ(intial_state, parser->tree.initial_state);
        ASSERT_EQ(states, parser->tree.states);
    }
}

struct TestInvalidParseStateList : public ::testing::Test {

    // Can add multiple cases to test

    std::vector<std::string> testCases;

    TestInvalidParseStateList() {
        testCases = {
            {"a,"},       // ending with comma
            {"a, [b"},    // incomplete bracket
            {"[a], [b]"}, // two initial states
            {"a, b"}      // no initial state
        };
    }

  protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TestInvalidParseStateList, sample_test) {
    for (auto src : testCases) {
        auto lexer = std::make_unique<lexer::Lexer>(src, false);
        auto parser = std::make_unique<parser::Parser>(std::move(lexer));
        EXPECT_THROW(parser->parse(), std::runtime_error);
    }
}
