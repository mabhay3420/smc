#ifndef LEXER_HPP
#define LEXER_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace lexer {

enum TokenType {
    EOF_TOKEN = -1,
    NEWLINE = 0,

    // Keywords
    STATES = 201,
    SYMBOLS = 202,
    TRANSITIONS = 203,
    // Contextual Keywords
    R = 104,
    L = 105,
    X = 106,
    P = 107,

    // Identifiers : Alphanumerics
    IDENT = 7,

    // Operators
    OR = 8,
    LeftBracket = 9,
    RightBracket = 10,
    COMMA = 11,
    DASH = 12,
    LeftParen = 13,
    RightParen = 14,
    STAR = 15,
    COLON = 16,
};

class Location {
  public:
    uint32_t line;
    uint32_t col;

    Location(uint32_t line, uint32_t col);
};

std::optional<TokenType> token_type_from_string(const std::string &s);
class Token {
  public:
    std::string token;
    TokenType kind;
    Location loc;

    Token(std::string token, TokenType kind, Location loc);
    static std::optional<TokenType>
    check_if_keyword(const std::string &token_text);
};

class Lexer {
  private:
    std::vector<char> source;
    char curr_char;
    Location cursor;

    void skip_whitespace();
    void skip_comment();

    void abort(const std::string &message) const;

  public:
    //   TODO - Use explicit if needed
    Lexer(const std::string &source_text);

    // get's the char value at the current cursor
    char curr_char_value() const;
    // moves cursor to the next character
    void next_char();

    // peeks ahead to look the character at position ahead of
    // cursor
    char peek() const;

    // Token generation
    std::optional<Token> get_token();
};

void log_info(const std::string &message);
void log_error(const std::string &message);

} // namespace lexer

#endif