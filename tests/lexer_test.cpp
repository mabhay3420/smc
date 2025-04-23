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
        fileNames = {"tests/examples/minimal1.sm", "tests/examples/comments.sm"};
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
      },
      {
        fileNames[1],
        {
            // Line 1: comment “# States ”
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[0], {{  9,1,10}, { 10,1,11}}},
    
            // Line 2: STATES: [b]
            {"STATES",       lexer::TokenType::STATES,      fileNames[0], {{ 10,2,1}, { 16,2,7}}},
            {":",            lexer::TokenType::COLON,       fileNames[0], {{ 16,2,7}, { 17,2,8}}},
            {"[",            lexer::TokenType::LeftBracket, fileNames[0], {{ 18,2,9}, { 19,2,10}}},
            {"b",            lexer::TokenType::IDENT,       fileNames[0], {{ 19,2,10},{ 20,2,11}}},
            {"]",            lexer::TokenType::RightBracket,fileNames[0], {{ 20,2,11},{ 21,2,12}}},
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[0], {{ 21,2,12},{ 22,2,13}}},
    
            // Line 3: blank
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[0], {{ 22,3,1}, { 23,3,2}}},
    
            // Line 4: comment “# Symbols”
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[0], {{ 32,4,10},{ 33,4,11}}},
    
            // Line 5: SYMBOLS: e
            {"SYMBOLS",      lexer::TokenType::SYMBOLS,     fileNames[0], {{ 33,5,1}, { 40,5,8}}},
            {":",            lexer::TokenType::COLON,       fileNames[0], {{ 40,5,8}, { 41,5,9}}},
            {"e",            lexer::TokenType::IDENT,       fileNames[0], {{ 42,5,10},{ 43,5,11}}},
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[0], {{ 43,5,11},{ 44,5,12}}},
    
            // Line 6: blank
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[0], {{ 44,6,1}, { 45,6,2}}},
    
            // Line 7: comment “# Transitions”
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[0], {{ 58,7,14},{ 59,7,15}}},
    
            // Line 8: TRANSITIONS:
            {"TRANSITIONS",  lexer::TokenType::TRANSITIONS, fileNames[0], {{ 59,8,1}, { 70,8,12}}},
            {":",            lexer::TokenType::COLON,       fileNames[0], {{ 70,8,12},{ 71,8,13}}},
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[0], {{ 71,8,13},{ 72,8,14}}},
    
            // Line 9: b,e,X,b
            {"b",            lexer::TokenType::IDENT,       fileNames[0], {{ 72,9,1}, { 73,9,2}}},
            {",",            lexer::TokenType::COMMA,       fileNames[0], {{ 73,9,2}, { 74,9,3}}},
            {"e",            lexer::TokenType::IDENT,       fileNames[0], {{ 74,9,3}, { 75,9,4}}},
            {",",            lexer::TokenType::COMMA,       fileNames[0], {{ 75,9,4}, { 76,9,5}}},
            {"X",            lexer::TokenType::X,           fileNames[0], {{ 76,9,5}, { 77,9,6}}},
            {",",            lexer::TokenType::COMMA,       fileNames[0], {{ 77,9,6}, { 78,9,7}}},
            {"b",            lexer::TokenType::IDENT,       fileNames[0], {{ 78,9,7}, { 79,9,8}}},
    
            // End-of-file
            {"\0",           lexer::TokenType::EOF_TOKEN,   fileNames[0], {{ 79,9,8}, { 80,9,9}}}
        }
    }
    
    };
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

// struct TestInvalidSources : public ::testing::Test {

//     // Can add multiple cases to test

//     std::vector<std::string> fileNames;
//     std::vector<std::tuple<std::string, lexer::Location>> invalidLocations;

//     TestInvalidSources() {
//         fileNames = {"tests/examples/invalid1.sm"};
//         // clang-format off
//       invalidLocations = {
//         {fileNames[0], {0, 0, 0}}
//       };
//         // clang-format on
//     }

//   protected:
//     void SetUp() override {}
//     void TearDown() override {}
// };

// TEST_F(TestInvalidSources, sample_test) {
//     for (auto [fileName, loc] : invalidLocations) {
//         auto lexer = std::make_unique<lexer::Lexer>(fileName);
//         std::vector<lexer::Token> tokenList;
//         bool foundInvalid = false;
//         while (1) {
//             // tokenList.push_back(token.value());
//             auto token = lexer->get_token();
//             if (!token) {
//                 ASSERT_EQ(loc, lexer->get_cursor());
//                 foundInvalid = true;
//             }
//             // End of file
//             if (token->kind == lexer::TokenType::EOF_TOKEN)
//                 break;
//         }
//         ASSERT_TRUE(foundInvalid);
//     }
// }