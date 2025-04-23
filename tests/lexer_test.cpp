#include "lexer.hpp"
#include <cstddef>
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
        // clang-format off
        fileTokenPairs = {{
          fileNames[0],
          {
              // Line 1: STATES: [b]
              {"STATES",     lexer::TokenType::STATES,      fileNames[0], {{ 0,1,1}, { 6,1,7}}},
              {":",          lexer::TokenType::COLON,       fileNames[0], {{ 6,1,7}, { 7,1,8}}},
              {"[",          lexer::TokenType::LeftBracket, fileNames[0], {{ 8,1,9}, { 9,1,10}}},
              {"b",          lexer::TokenType::IDENT,       fileNames[0], {{ 9,1,10},{10,1,11}}},
              {"]",          lexer::TokenType::RightBracket,fileNames[0], {{10,1,11},{11,1,12}}},
              {"\n",         lexer::TokenType::NEWLINE,     fileNames[0], {{11,1,12},{12,1,13}}},
      
              // Line 2: SYMBOLS: e
              {"SYMBOLS",    lexer::TokenType::SYMBOLS,     fileNames[0], {{12,2,1}, {19,2,8}}},
              {":",          lexer::TokenType::COLON,       fileNames[0], {{19,2,8}, {20,2,9}}},
              {"e",          lexer::TokenType::IDENT,       fileNames[0], {{21,2,10},{22,2,11}}},
              {"\n",         lexer::TokenType::NEWLINE,     fileNames[0], {{22,2,11},{23,2,12}}},
      
              // Line 3: TRANSITIONS:
              {"TRANSITIONS",lexer::TokenType::TRANSITIONS, fileNames[0], {{23,3,1}, {34,3,12}}},
              {":",          lexer::TokenType::COLON,       fileNames[0], {{34,3,12},{35,3,13}}},
              {"\n",         lexer::TokenType::NEWLINE,     fileNames[0], {{35,3,13},{36,3,14}}},
      
              // Line 4: b,e,X,b
              {"b",          lexer::TokenType::IDENT,       fileNames[0], {{36,4,1}, {37,4,2}}},
              {",",          lexer::TokenType::COMMA,       fileNames[0], {{37,4,2}, {38,4,3}}},
              {"e",          lexer::TokenType::IDENT,       fileNames[0], {{38,4,3}, {39,4,4}}},
              {",",          lexer::TokenType::COMMA,       fileNames[0], {{39,4,4}, {40,4,5}}},
              {"X",          lexer::TokenType::X,           fileNames[0], {{40,4,5}, {41,4,6}}},
              {",",          lexer::TokenType::COMMA,       fileNames[0], {{41,4,6}, {42,4,7}}},
              {"b",          lexer::TokenType::IDENT,       fileNames[0], {{42,4,7}, {43,4,8}}},
              // End‐of‐file
              {"\0",           lexer::TokenType::EOF_TOKEN,   fileNames[0], {{43,4,8}, {44,4,9}}}
          }
      }};
        // clang-format on
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
        ASSERT_EQ(tokenList.size(), expectedTokenList.size());
        ASSERT_EQ(tokenList, expectedTokenList);
    }
}