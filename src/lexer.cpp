#include "lexer.hpp"
#include <cstdint>
#include <map>
#include <optional>
#include <string>

namespace lexer {

const std::map<std::string, TokenType> tokenMap = {
    {"EOF", TokenType::EOF_TOKEN},
    {"NEWLINE", TokenType::NEWLINE},
    {"STATES", TokenType::STATES},
    {"SYMBOLS", TokenType::SYMBOLS},
    {"TRANSITIONS", TokenType::TRANSITIONS},
    {"R", TokenType::R},
    {"L", TokenType::L},
    {"X", TokenType::X},
    {"P", TokenType::P},
    {"IDENT", TokenType::IDENT},
    {"OR", TokenType::OR},
    {"[", TokenType::LeftBracket},
    {"]", TokenType::RightBracket},
    {",", TokenType::COMMA},
    {"-", TokenType::DASH},
    {"(", TokenType::LeftParen},
    {")", TokenType::RightParen},
    {"*", TokenType::STAR},
    {":", TokenType::COLON}};

const std::map<TokenType, std::string> tokenToString = {
    {TokenType::EOF_TOKEN, "EOF"},
    {TokenType::NEWLINE, "NEWLINE"},
    {TokenType::STATES, "STATES"},
    {TokenType::SYMBOLS, "SYMBOLS"},
    {TokenType::TRANSITIONS, "TRANSITIONS"},
    {TokenType::R, "R"},
    {TokenType::L, "L"},
    {TokenType::X, "X"},
    {TokenType::P, "P"},
    {TokenType::IDENT, "IDENT"},
    {TokenType::OR, "OR"},
    {TokenType::LeftBracket, "["},
    {TokenType::RightBracket, "]"},
    {TokenType::COMMA, ","},
    {TokenType::DASH, "-"},
    {TokenType::LeftParen, "("},
    {TokenType::RightParen, ")"},
    {TokenType::STAR, "*"},
    {TokenType::COLON, ":"}};

std::ostream &operator<<(std::ostream &os, const TokenType &t) {
    os << "TT: '" << token_type_to_string(t).value_or("") << "' ("
       << static_cast<int>(t) << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const LocationRange &range) {
    os << range.start << "-" << range.end;
    return os;
}

std::ostream &operator<<(std::ostream &os, const Location &loc) {
    os << loc.line << ":" << loc.col;
    return os;
};

Location operator+(const Location &loc, uint32_t offset) {
    Location newLoc(loc.index + offset, loc.line, loc.col + offset);
    return newLoc;
}
std::ostream &operator<<(std::ostream &os, const Token &token) {
    Indent indent{1};
    os << "Token {"
       << "\n";
    os << indent << token.token << "\n";
    os << indent << token.kind << "\n";
    if (token.range.start.line == token.range.end.line) {
        // pretty print case
        os << indent << token.filename << ":" << token.range.start.line << ":"
           << token.range.start.col << "-" << token.range.end.col << "\n";
    } else {
        os << indent << token.range.start << "-" << token.range.end << "\n";
    }
    os << "}";
    return os;
};

const std::optional<TokenType> token_type_from_string(const std::string &s) {
    auto it = tokenMap.find(s);
    if (it != tokenMap.end()) {
        return it->second;
    }
    return std::nullopt;
}
const std::optional<std::string> token_type_to_string(const TokenType kind) {
    auto it = tokenToString.find(kind);
    if (it != tokenToString.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<TokenType>
Token::check_if_keyword(const std::string &token_text) {
    auto tokenType = token_type_from_string(token_text);
    if (tokenType.has_value()) {
        auto tokenId = static_cast<uint32_t>(tokenType.value());

        if (tokenId >= KEYWORD_START) {
            return tokenType.value();
        }
    }
    return std::nullopt;
}
void log_info(const std::string &message) {
    std::cout << "[INFO] Lexer: " << message << std::endl;
};
void log_error(const std::string &message) {
    std::cout << "[ERROR] Lexer: " << message << std::endl;
};
} // namespace lexer
