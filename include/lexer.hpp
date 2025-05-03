#ifndef LEXER_HPP
#define LEXER_HPP

#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "utils.hpp"

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
const uint32_t KEYWORD_START = 104;

const std::optional<TokenType> token_type_from_string(const std::string &s);

const std::optional<std::string> token_type_to_string(const TokenType kind);

std::ostream &operator<<(std::ostream &os, const TokenType &t);

class Location {
  public:
    uint32_t line;
    uint32_t col;
    uint32_t index;

    Location(){};

    Location(uint32_t index, uint32_t line, uint32_t col)
        : line(line), col(col), index(index) {}

    bool operator==(const Location &loc) const {
        return (loc.line == line) & (loc.col == col) && (loc.index == index);
    }
};
Location operator+(const Location &loc, uint32_t offset);

class LocationRange {
  public:
    Location start;
    Location end;
    LocationRange(){};
    LocationRange(const Location &start, const Location &end)
        : start(start), end(end){};
    bool operator==(const LocationRange &range) const {
        return range.start == start && range.end == end;
    }
};

std::ostream &operator<<(std::ostream &os, const LocationRange &range);
std::ostream &operator<<(std::ostream &os, const Location &loc);

class Token {
  public:
    std::string token;
    TokenType kind;
    std::string filename;
    LocationRange range;
    Token(){};
    Token(std::string token, TokenType kind, std::string filename,
          LocationRange range)
        : token(token), kind(kind), filename(filename), range(range){};
    static std::optional<TokenType>
    check_if_keyword(const std::string &token_text);

    bool operator==(const Token &rightToken) const {
        return (rightToken.token == token) && (rightToken.kind == kind) &&
               rightToken.range == range && (rightToken.filename == filename);
    }
};

std::ostream &operator<<(std::ostream &os, const Token &token);

void log_info(const std::string &message);
void log_error(const std::string &message);
class Lexer {
  private:
    std::vector<char> source;
    char curr_char;
    Location cursor;

    void skip_whitespace() {
        while (curr_char == ' ' or curr_char == '\t') {
            next_char();
        }
    };
    // single line comment
    // check at the start of each line
    void skip_comment() {
        assert(cursor.col == 1);
        skip_whitespace();
        // if at the start of the line
        if (curr_char == '#') {
            // skip this line
            while (curr_char != '\n')
                next_char();
        }
    };

    void abort(const std::string &message) const {
        throw std::runtime_error("Lexer Error: " + message);
    };

  public:
    std::string srcfile;
    //   TODO - Use explicit if needed
    Lexer(std::string srcfile)
        : srcfile(srcfile), cursor(0, 1, 1) {
        auto source_text = read_file_to_string(srcfile);
        source.reserve(source_text.size());
        for (char c : source_text) {
            source.push_back(c);
        }
        source.push_back('\0');
        curr_char = source[0];
    };

    Lexer(const std::string &source_text, bool with_file)
        : srcfile(""), cursor(0, 1, 1) {
        // source.push_back('\0');
        source.reserve(source_text.size());
        for (char c : source_text)
            source.push_back(c);
        source.push_back('\0');
        curr_char = source[0];
    };

    // get's the char value at the current cursor
    char get_curr_char() const { return curr_char; };
    Location get_cursor() const { return cursor; }
    // moves cursor to the next character
    void next_char() {
        // update cursor
        if (curr_char == '\n') {
            cursor.line++;
            cursor.col = 1;
        } else {
            cursor.col++;
        }
        // move to next character
        curr_char = source[++cursor.index];
    };

    // peeks ahead to look the character at position ahead of
    // cursor
    char peek() const {
        // nothing to peek ahead
        if (curr_char == '\0')
            return '\0';
        return source[cursor.index + 1];
    };

    // Token generation
    // starting from current token try to match next token
    std::optional<Token> get_token() {
        // check for comment at the start of each line
        if (cursor.col == 1)
            skip_comment();
        // skip whitespace
        skip_whitespace();

        // Store the current cursor position for the token
        Location token_loc = cursor;

        // match directly
        switch (curr_char) {
        case '\0':
            next_char();
            return Token("\0", TokenType::EOF_TOKEN, srcfile,
                         {token_loc, token_loc + 1});
        case '\n':
            next_char();
            return Token("\n", TokenType::NEWLINE, srcfile,
                         {token_loc, token_loc + 1});
        case '|':
            next_char();
            return Token("|", TokenType::OR, srcfile,
                         {token_loc, token_loc + 1});
        case '[':
            next_char();
            return Token("[", TokenType::LeftBracket, srcfile,
                         {token_loc, token_loc + 1});
        case ']':
            next_char();
            return Token("]", TokenType::RightBracket, srcfile,
                         {token_loc, token_loc + 1});
        case ',':
            next_char();
            return Token(",", TokenType::COMMA, srcfile,
                         {token_loc, token_loc + 1});
        case '-':
            next_char();
            return Token("-", TokenType::DASH, srcfile,
                         {token_loc, token_loc + 1});
        case '(':
            next_char();
            return Token("(", TokenType::LeftParen, srcfile,
                         {token_loc, token_loc + 1});
        case ')':
            next_char();
            return Token(")", TokenType::RightParen, srcfile,
                         {token_loc, token_loc + 1});
        case '*':
            next_char();
            return Token("*", TokenType::STAR, srcfile,
                         {token_loc, token_loc + 1});
        case ':':
            next_char();
            return Token(":", TokenType::COLON, srcfile,
                         {token_loc, token_loc + 1});
        default:
            // Handle alphanumeric identifiers and keywords
            if (std::isalnum(curr_char)) {
                std::string token_text;
                token_text += curr_char;

                next_char();
                while (std::isalnum(curr_char)) {
                    token_text += curr_char;
                    next_char();
                }

                // Check if it's a keyword
                auto keyword = Token::check_if_keyword(token_text);
                if (keyword.has_value()) {
                    return Token(token_text, keyword.value(), srcfile,
                                 {token_loc, cursor});
                } else {
                    return Token(token_text, TokenType::IDENT, srcfile,
                                 {token_loc, cursor});
                }
            } else {
                // Unknown token
                std::string message = "Unknown token: ";
                message += curr_char;
                log_error(message);
                return std::nullopt;
            }
        }
    }
};

} // namespace lexer

#endif