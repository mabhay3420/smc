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
        fileNames = {"tests/examples/minimal1.sm", "tests/examples/comments.sm",
                     "tests/examples/simple2.sm"};
        // clang-format off
        fileTokenPairs = {
    {
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
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[1], {{  9,1,10}, { 10,1,11}}},
    
            // Line 2: STATES: [b]
            {"STATES",       lexer::TokenType::STATES,      fileNames[1], {{ 10,2,1}, { 16,2,7}}},
            {":",            lexer::TokenType::COLON,       fileNames[1], {{ 16,2,7}, { 17,2,8}}},
            {"[",            lexer::TokenType::LeftBracket, fileNames[1], {{ 18,2,9}, { 19,2,10}}},
            {"b",            lexer::TokenType::IDENT,       fileNames[1], {{ 19,2,10},{ 20,2,11}}},
            {"]",            lexer::TokenType::RightBracket,fileNames[1], {{ 20,2,11},{ 21,2,12}}},
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[1], {{ 21,2,12},{ 22,2,13}}},
    
            // Line 3: blank
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[1], {{ 22,3,1}, { 23,3,2}}},
    
            // Line 4: comment “# Symbols”
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[1], {{ 32,4,10},{ 33,4,11}}},
    
            // Line 5: SYMBOLS: e
            {"SYMBOLS",      lexer::TokenType::SYMBOLS,     fileNames[1], {{ 33,5,1}, { 40,5,8}}},
            {":",            lexer::TokenType::COLON,       fileNames[1], {{ 40,5,8}, { 41,5,9}}},
            {"e",            lexer::TokenType::IDENT,       fileNames[1], {{ 42,5,10},{ 43,5,11}}},
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[1], {{ 43,5,11},{ 44,5,12}}},
    
            // Line 6: blank
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[1], {{ 44,6,1}, { 45,6,2}}},
    
            // Line 7: comment “# Transitions”
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[1], {{ 58,7,14},{ 59,7,15}}},
    
            // Line 8: TRANSITIONS:
            {"TRANSITIONS",  lexer::TokenType::TRANSITIONS, fileNames[1], {{ 59,8,1}, { 70,8,12}}},
            {":",            lexer::TokenType::COLON,       fileNames[1], {{ 70,8,12},{ 71,8,13}}},
            {"\n",           lexer::TokenType::NEWLINE,     fileNames[1], {{ 71,8,13},{ 72,8,14}}},
    
            // Line 9: b,e,X,b
            {"b",            lexer::TokenType::IDENT,       fileNames[1], {{ 72,9,1}, { 73,9,2}}},
            {",",            lexer::TokenType::COMMA,       fileNames[1], {{ 73,9,2}, { 74,9,3}}},
            {"e",            lexer::TokenType::IDENT,       fileNames[1], {{ 74,9,3}, { 75,9,4}}},
            {",",            lexer::TokenType::COMMA,       fileNames[1], {{ 75,9,4}, { 76,9,5}}},
            {"X",            lexer::TokenType::X,           fileNames[1], {{ 76,9,5}, { 77,9,6}}},
            {",",            lexer::TokenType::COMMA,       fileNames[1], {{ 77,9,6}, { 78,9,7}}},
            {"b",            lexer::TokenType::IDENT,       fileNames[1], {{ 78,9,7}, { 79,9,8}}},
    
            // End-of-file
            {"\0",           lexer::TokenType::EOF_TOKEN,   fileNames[1], {{ 79,9,8}, { 80,9,9}}}
        }
      },
      {
        fileNames[2],
          {
            { "STATES", lexer::TokenType::STATES, fileNames[2], { { 0,1,1 }, { 6,1,7 } } },
            { ":", lexer::TokenType::COLON, fileNames[2], { { 6,1,7 }, { 7,1,8 } } },
            { "[", lexer::TokenType::LeftBracket, fileNames[2], { { 8,1,9 }, { 9,1,10 } } },
            { "b", lexer::TokenType::IDENT, fileNames[2], { { 9,1,10 }, { 10,1,11 } } },
            { "]", lexer::TokenType::RightBracket, fileNames[2], { { 10,1,11 }, { 11,1,12 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 11,1,12 }, { 12,1,13 } } },
            { "o", lexer::TokenType::IDENT, fileNames[2], { { 13,1,14 }, { 14,1,15 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 14,1,15 }, { 15,1,16 } } },
            { "q", lexer::TokenType::IDENT, fileNames[2], { { 16,1,17 }, { 17,1,18 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 17,1,18 }, { 18,1,19 } } },
            { "p", lexer::TokenType::IDENT, fileNames[2], { { 19,1,20 }, { 20,1,21 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 20,1,21 }, { 21,1,22 } } },
            { "f", lexer::TokenType::IDENT, fileNames[2], { { 22,1,23 }, { 23,1,24 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 23,1,24 }, { 24,1,25 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 24,2,1 }, { 25,2,2 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 44,3,20 }, { 45,3,21 } } },
            { "SYMBOLS", lexer::TokenType::SYMBOLS, fileNames[2], { { 45,4,1 }, { 52,4,8 } } },
            { ":", lexer::TokenType::COLON, fileNames[2], { { 52,4,8 }, { 53,4,9 } } },
            { "0", lexer::TokenType::IDENT, fileNames[2], { { 54,4,10 }, { 55,4,11 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 55,4,11 }, { 56,4,12 } } },
            { "1", lexer::TokenType::IDENT, fileNames[2], { { 57,4,13 }, { 58,4,14 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 58,4,14 }, { 59,4,15 } } },
            { "e", lexer::TokenType::IDENT, fileNames[2], { { 60,4,16 }, { 61,4,17 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 61,4,17 }, { 62,4,18 } } },
            { "x", lexer::TokenType::IDENT, fileNames[2], { { 63,4,19 }, { 64,4,20 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 64,4,20 }, { 65,4,21 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 65,5,1 }, { 66,5,2 } } },
            { "TRANSITIONS", lexer::TokenType::TRANSITIONS, fileNames[2], { { 66,6,1 }, { 77,6,12 } } },
            { ":", lexer::TokenType::COLON, fileNames[2], { { 77,6,12 }, { 78,6,13 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 78,6,13 }, { 79,6,14 } } },
            { "b", lexer::TokenType::IDENT, fileNames[2], { { 79,7,1 }, { 80,7,2 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 80,7,2 }, { 81,7,3 } } },
            { "*", lexer::TokenType::STAR, fileNames[2], { { 82,7,4 }, { 83,7,5 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 83,7,5 }, { 84,7,6 } } },
            { "P", lexer::TokenType::P, fileNames[2], { { 85,7,7 }, { 86,7,8 } } },
            { "(", lexer::TokenType::LeftParen, fileNames[2], { { 86,7,8 }, { 87,7,9 } } },
            { "e", lexer::TokenType::IDENT, fileNames[2], { { 87,7,9 }, { 88,7,10 } } },
            { ")", lexer::TokenType::RightParen, fileNames[2], { { 88,7,10 }, { 89,7,11 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 89,7,11 }, { 90,7,12 } } },
            { "R", lexer::TokenType::R, fileNames[2], { { 90,7,12 }, { 91,7,13 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 91,7,13 }, { 92,7,14 } } },
            { "P", lexer::TokenType::P, fileNames[2], { { 92,7,14 }, { 93,7,15 } } },
            { "(", lexer::TokenType::LeftParen, fileNames[2], { { 93,7,15 }, { 94,7,16 } } },
            { "e", lexer::TokenType::IDENT, fileNames[2], { { 94,7,16 }, { 95,7,17 } } },
            { ")", lexer::TokenType::RightParen, fileNames[2], { { 95,7,17 }, { 96,7,18 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 96,7,18 }, { 97,7,19 } } },
            { "R", lexer::TokenType::R, fileNames[2], { { 97,7,19 }, { 98,7,20 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 98,7,20 }, { 99,7,21 } } },
            { "P", lexer::TokenType::P, fileNames[2], { { 99,7,21 }, { 100,7,22 } } },
            { "(", lexer::TokenType::LeftParen, fileNames[2], { { 100,7,22 }, { 101,7,23 } } },
            { "0", lexer::TokenType::IDENT, fileNames[2], { { 101,7,23 }, { 102,7,24 } } },
            { ")", lexer::TokenType::RightParen, fileNames[2], { { 102,7,24 }, { 103,7,25 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 103,7,25 }, { 104,7,26 } } },
            { "R", lexer::TokenType::R, fileNames[2], { { 104,7,26 }, { 105,7,27 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 105,7,27 }, { 106,7,28 } } },
            { "R", lexer::TokenType::R, fileNames[2], { { 106,7,28 }, { 107,7,29 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 107,7,29 }, { 108,7,30 } } },
            { "P", lexer::TokenType::P, fileNames[2], { { 108,7,30 }, { 109,7,31 } } },
            { "(", lexer::TokenType::LeftParen, fileNames[2], { { 109,7,31 }, { 110,7,32 } } },
            { "0", lexer::TokenType::IDENT, fileNames[2], { { 110,7,32 }, { 111,7,33 } } },
            { ")", lexer::TokenType::RightParen, fileNames[2], { { 111,7,33 }, { 112,7,34 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 112,7,34 }, { 113,7,35 } } },
            { "L", lexer::TokenType::L, fileNames[2], { { 113,7,35 }, { 114,7,36 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 114,7,36 }, { 115,7,37 } } },
            { "L", lexer::TokenType::L, fileNames[2], { { 115,7,37 }, { 116,7,38 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 116,7,38 }, { 117,7,39 } } },
            { "o", lexer::TokenType::IDENT, fileNames[2], { { 118,7,40 }, { 119,7,41 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 119,7,41 }, { 120,7,42 } } },
            { "o", lexer::TokenType::IDENT, fileNames[2], { { 120,8,1 }, { 121,8,2 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 121,8,2 }, { 122,8,3 } } },
            { "1", lexer::TokenType::IDENT, fileNames[2], { { 123,8,4 }, { 124,8,5 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 124,8,5 }, { 125,8,6 } } },
            { "R", lexer::TokenType::R, fileNames[2], { { 126,8,7 }, { 127,8,8 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 127,8,8 }, { 128,8,9 } } },
            { "P", lexer::TokenType::P, fileNames[2], { { 128,8,9 }, { 129,8,10 } } },
            { "(", lexer::TokenType::LeftParen, fileNames[2], { { 129,8,10 }, { 130,8,11 } } },
            { "x", lexer::TokenType::IDENT, fileNames[2], { { 130,8,11 }, { 131,8,12 } } },
            { ")", lexer::TokenType::RightParen, fileNames[2], { { 131,8,12 }, { 132,8,13 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 132,8,13 }, { 133,8,14 } } },
            { "L", lexer::TokenType::L, fileNames[2], { { 133,8,14 }, { 134,8,15 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 134,8,15 }, { 135,8,16 } } },
            { "L", lexer::TokenType::L, fileNames[2], { { 135,8,16 }, { 136,8,17 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 136,8,17 }, { 137,8,18 } } },
            { "L", lexer::TokenType::L, fileNames[2], { { 137,8,18 }, { 138,8,19 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 138,8,19 }, { 139,8,20 } } },
            { "o", lexer::TokenType::IDENT, fileNames[2], { { 140,8,21 }, { 141,8,22 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 141,8,22 }, { 142,8,23 } } },
            { "o", lexer::TokenType::IDENT, fileNames[2], { { 142,9,1 }, { 143,9,2 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 143,9,2 }, { 144,9,3 } } },
            { "0", lexer::TokenType::IDENT, fileNames[2], { { 145,9,4 }, { 146,9,5 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 146,9,5 }, { 147,9,6 } } },
            { "X", lexer::TokenType::X, fileNames[2], { { 148,9,7 }, { 149,9,8 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 149,9,8 }, { 150,9,9 } } },
            { "q", lexer::TokenType::IDENT, fileNames[2], { { 151,9,10 }, { 152,9,11 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 152,9,11 }, { 153,9,12 } } },
            { "q", lexer::TokenType::IDENT, fileNames[2], { { 153,10,1 }, { 154,10,2 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 154,10,2 }, { 155,10,3 } } },
            { "0", lexer::TokenType::IDENT, fileNames[2], { { 156,10,4 }, { 157,10,5 } } },
            { "|", lexer::TokenType::OR, fileNames[2], { { 158,10,6 }, { 159,10,7 } } },
            { "1", lexer::TokenType::IDENT, fileNames[2], { { 160,10,8 }, { 161,10,9 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 161,10,9 }, { 162,10,10 } } },
            { "R", lexer::TokenType::R, fileNames[2], { { 163,10,11 }, { 164,10,12 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 164,10,12 }, { 165,10,13 } } },
            { "R", lexer::TokenType::R, fileNames[2], { { 165,10,13 }, { 166,10,14 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 166,10,14 }, { 167,10,15 } } },
            { "q", lexer::TokenType::IDENT, fileNames[2], { { 168,10,16 }, { 169,10,17 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 169,10,17 }, { 170,10,18 } } },
            { "q", lexer::TokenType::IDENT, fileNames[2], { { 170,11,1 }, { 171,11,2 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 171,11,2 }, { 172,11,3 } } },
            { "X", lexer::TokenType::X, fileNames[2], { { 173,11,4 }, { 174,11,5 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 174,11,5 }, { 175,11,6 } } },
            { "P", lexer::TokenType::P, fileNames[2], { { 176,11,7 }, { 177,11,8 } } },
            { "(", lexer::TokenType::LeftParen, fileNames[2], { { 177,11,8 }, { 178,11,9 } } },
            { "1", lexer::TokenType::IDENT, fileNames[2], { { 178,11,9 }, { 179,11,10 } } },
            { ")", lexer::TokenType::RightParen, fileNames[2], { { 179,11,10 }, { 180,11,11 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 180,11,11 }, { 181,11,12 } } },
            { "L", lexer::TokenType::L, fileNames[2], { { 181,11,12 }, { 182,11,13 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 182,11,13 }, { 183,11,14 } } },
            { "p", lexer::TokenType::IDENT, fileNames[2], { { 184,11,15 }, { 185,11,16 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 185,11,16 }, { 186,11,17 } } },
            { "p", lexer::TokenType::IDENT, fileNames[2], { { 186,12,1 }, { 187,12,2 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 187,12,2 }, { 188,12,3 } } },
            { "x", lexer::TokenType::IDENT, fileNames[2], { { 189,12,4 }, { 190,12,5 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 190,12,5 }, { 191,12,6 } } },
            { "P", lexer::TokenType::P, fileNames[2], { { 192,12,7 }, { 193,12,8 } } },
            { "(", lexer::TokenType::LeftParen, fileNames[2], { { 193,12,8 }, { 194,12,9 } } },
            { "X", lexer::TokenType::X, fileNames[2], { { 194,12,9 }, { 195,12,10 } } },
            { ")", lexer::TokenType::RightParen, fileNames[2], { { 195,12,10 }, { 196,12,11 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 196,12,11 }, { 197,12,12 } } },
            { "R", lexer::TokenType::R, fileNames[2], { { 197,12,12 }, { 198,12,13 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 198,12,13 }, { 199,12,14 } } },
            { "q", lexer::TokenType::IDENT, fileNames[2], { { 200,12,15 }, { 201,12,16 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 201,12,16 }, { 202,12,17 } } },
            { "p", lexer::TokenType::IDENT, fileNames[2], { { 202,13,1 }, { 203,13,2 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 203,13,2 }, { 204,13,3 } } },
            { "e", lexer::TokenType::IDENT, fileNames[2], { { 205,13,4 }, { 206,13,5 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 206,13,5 }, { 207,13,6 } } },
            { "R", lexer::TokenType::R, fileNames[2], { { 208,13,7 }, { 209,13,8 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 209,13,8 }, { 210,13,9 } } },
            { "f", lexer::TokenType::IDENT, fileNames[2], { { 211,13,10 }, { 212,13,11 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 212,13,11 }, { 213,13,12 } } },
            { "p", lexer::TokenType::IDENT, fileNames[2], { { 213,14,1 }, { 214,14,2 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 214,14,2 }, { 215,14,3 } } },
            { "X", lexer::TokenType::X, fileNames[2], { { 216,14,4 }, { 217,14,5 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 217,14,5 }, { 218,14,6 } } },
            { "L", lexer::TokenType::L, fileNames[2], { { 219,14,7 }, { 220,14,8 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 220,14,8 }, { 221,14,9 } } },
            { "L", lexer::TokenType::L, fileNames[2], { { 221,14,9 }, { 222,14,10 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 222,14,10 }, { 223,14,11 } } },
            { "p", lexer::TokenType::IDENT, fileNames[2], { { 224,14,12 }, { 225,14,13 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 225,14,13 }, { 226,14,14 } } },
            { "f", lexer::TokenType::IDENT, fileNames[2], { { 226,15,1 }, { 227,15,2 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 227,15,2 }, { 228,15,3 } } },
            { "*", lexer::TokenType::STAR, fileNames[2], { { 229,15,4 }, { 230,15,5 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 230,15,5 }, { 231,15,6 } } },
            { "R", lexer::TokenType::R, fileNames[2], { { 232,15,7 }, { 233,15,8 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 233,15,8 }, { 234,15,9 } } },
            { "R", lexer::TokenType::R, fileNames[2], { { 234,15,9 }, { 235,15,10 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 235,15,10 }, { 236,15,11 } } },
            { "f", lexer::TokenType::IDENT, fileNames[2], { { 237,15,12 }, { 238,15,13 } } },
            { "\n", lexer::TokenType::NEWLINE, fileNames[2], { { 238,15,13 }, { 239,15,14 } } },
            { "f", lexer::TokenType::IDENT, fileNames[2], { { 239,16,1 }, { 240,16,2 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 240,16,2 }, { 241,16,3 } } },
            { "X", lexer::TokenType::X, fileNames[2], { { 242,16,4 }, { 243,16,5 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 243,16,5 }, { 244,16,6 } } },
            { "P", lexer::TokenType::P, fileNames[2], { { 245,16,7 }, { 246,16,8 } } },
            { "(", lexer::TokenType::LeftParen, fileNames[2], { { 246,16,8 }, { 247,16,9 } } },
            { "0", lexer::TokenType::IDENT, fileNames[2], { { 247,16,9 }, { 248,16,10 } } },
            { ")", lexer::TokenType::RightParen, fileNames[2], { { 248,16,10 }, { 249,16,11 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 249,16,11 }, { 250,16,12 } } },
            { "L", lexer::TokenType::L, fileNames[2], { { 250,16,12 }, { 251,16,13 } } },
            { "-", lexer::TokenType::DASH, fileNames[2], { { 251,16,13 }, { 252,16,14 } } },
            { "L", lexer::TokenType::L, fileNames[2], { { 252,16,14 }, { 253,16,15 } } },
            { ",", lexer::TokenType::COMMA, fileNames[2], { { 253,16,15 }, { 254,16,16 } } },
            { "o", lexer::TokenType::IDENT, fileNames[2], { { 255,16,17 }, { 256,16,18 } } },
            { "\0", lexer::TokenType::EOF_TOKEN, fileNames[2], { { 256,16,18 }, { 257,16,19 } } },
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
        for (size_t i = 0; i < tokenList.size(); i++) {
            std::cout << "i: " << i << std::endl;
            ASSERT_EQ(tokenList[i], expectedTokenList[i]);
        }
        ASSERT_EQ(tokenList, expectedTokenList);
    }
}

struct TestInvalidSources : public ::testing::Test {

    // Can add multiple cases to test

    std::vector<std::string> fileNames;
    std::vector<std::tuple<std::string, lexer::Location>> invalidLocations;

    TestInvalidSources() {
        fileNames = {"tests/examples/invalid1.sm",
                     "tests/examples/invalid2.sm"};
        // clang-format off
      invalidLocations = {
          // ‘#’ in “b,e,#,X,b” at byte-offset 40, line 4, column 5
          { fileNames[0], { 40, 4, 5 } },
          {fileNames[1], {40, 4, 5}}
      };
        // clang-format on
    }

  protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TestInvalidSources, sample_test) {
    for (auto [fileName, loc] : invalidLocations) {
        auto lexer = std::make_unique<lexer::Lexer>(fileName);
        bool foundInvalid = false;
        while (true) {
            auto token = lexer->get_token();
            if (!token) {
                // error happened exactly at the ‘#’
                ASSERT_EQ(loc, lexer->get_cursor());
                foundInvalid = true;
                break;
            }
            if (token->kind == lexer::TokenType::EOF_TOKEN)
                break;
        }
        ASSERT_TRUE(foundInvalid);
    }
}
