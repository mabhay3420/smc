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

std::ostream &operator<<(std::ostream &os, const TokenType &t) {
    os << "TT:" << static_cast<int>(t);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Location &loc) {
    os << loc.filename << ":" << loc.line << ":" << loc.col;
    return os;
};

Location operator+(const Location &loc, uint32_t offset) {
    Location newLoc(loc.filename, loc.index + offset, loc.line,
                    loc.col + offset);
    return newLoc;
}
std::ostream &operator<<(std::ostream &os, const Token &token) {
    Indent indent{1};
    os << "Token {"
       << "\n";
    os << indent << token.token << "\n";
    os << indent << token.kind << "\n";
    if (token.start.filename == token.end.filename &&
        token.start.line == token.end.line) {
        // pretty print case
        os << indent << token.start.filename << ":" << token.start.line << ":"
           << token.start.col << "-" << token.end.col << "\n";
    } else {
        os << indent << token.start << "-" << token.end << "\n";
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
