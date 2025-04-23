#include "lexer.hpp"
#include <gtest/gtest.h>
#include <string>
#include <tuple>
#include <vector>

//========================================================================
// Test Fixtures
//========================================================================

using TokenList = std::vector<lexer::Token>;
struct TestCorrectSources : public ::testing::Test {

    // Can add multiple cases to test

    std::vector<std::string> fileNames;
    std::vector<std::tuple<std::string, TokenList>> fileTokenPairs;

    TestCorrectSources() {
        fileNames = {"tests/examples/minimal1.sm"};
        fileTokenPairs = {{fileNames[0],
                           {{"STATES",
                             lexer::TokenType::STATES,
                             fileNames[0],
                             {{0, 1, 1}, {6, 1, 7}}}}}};
    }

  protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TestCorrectSources, sample_test) {
    for (auto [fileName, expectedTokenList] : fileTokenPairs) {
        auto lexer = std::make_unique<lexer::Lexer>(fileName);
        std::vector<lexer::Token> tokenList;

        while (auto token = lexer->get_token()) {
            tokenList.push_back(token.value());
            // End of file
            if (token->kind == lexer::TokenType::EOF_TOKEN)
                break;
        }
        ASSERT_GT(tokenList.size(), 0);
        ASSERT_EQ(tokenList[0], expectedTokenList[0]);
    }
}