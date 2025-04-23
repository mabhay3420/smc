#include "parser.hpp"
#include "lexer.hpp"
#include <optional>
#include <variant>

namespace parser {

std::optional<TransitionStep> fromTokenAndValue(const lexer::Token &token,
                                                const std::string &value) {
    switch (token.kind) {
    case lexer::TokenType::R:
        return R();
    case lexer::TokenType::L:
        return L();
    case lexer::TokenType::X:
        return X();
    case lexer::TokenType::P:
        // TODO - Must check whether the symbol is a valid symbol
        return P{value};
    default:
        return std::nullopt;
    }
}

} // namespace parser