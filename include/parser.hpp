#ifndef PARSER_HPP
#define PARSER_HPP
#include "lexer.hpp"
#include <algorithm>
#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <regex>
#include <shared_mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace parser {

struct Star {};
struct OR {
    std::vector<std::string> sym;
};

using Condition = std::variant<Star, OR>;

struct R {};
struct L {};
struct X {};
struct P {
    std::string sym;
};

using TransitionStep = std::variant<R, L, X, P>;

// TODO : See if we can specify a concept (C++ 20)
std::optional<TransitionStep> fromTokenAndValue(const lexer::Token &token,
                                                const std::string &value = "");

class Transition {
  public:
    std::string initialState;
    Condition condition;
    std::vector<TransitionStep> steps;
    std::string finalState;

    Transition()
        : initialState(""), condition(OR{{}}), steps({}), finalState(""){};
};

class ParseTree {
  public:
    std::vector<std::string> states;
    std::string initial_state;
    std::vector<std::string> symbols;
    std::vector<Transition> transitions;
};

template <typename T> using optionalFnType = std::optional<std::function<T>>;

class Parser {
  private:
    std::unique_ptr<lexer::Lexer> lexer;
    lexer::Token curr_token;
    lexer::Token peek_token;

  public:
    ParseTree tree;
    Parser(std::unique_ptr<lexer::Lexer> lexer) : lexer(std::move(lexer)) {
        nextToken(); // initialize peek_token
        nextToken(); // initialize curr_token
    }

    void nextToken() {
        curr_token = peek_token;
        auto tok = lexer->get_token();
        if (!tok) {
            throw std::runtime_error("Lexer found unexpected token - exiting");
        }
        peek_token = tok.value();
    }

    inline bool currHasType(lexer::TokenType ttype) {
        return curr_token.kind == ttype;
    }
    inline bool peekHasType(lexer::TokenType ttype) {
        return peek_token.kind == ttype;
    }
    using actionOnTokenFnType = optionalFnType<void(const lexer::Token &)>;

    // this will try to consume the next token if it can
    // otherwise it simply returns false
    // this should be called when there might or might not
    // be a token of certain kind present. Not being present
    // is not an error but rather a signal of sequence termination.
    bool try_consume(const lexer::TokenType &kind,
                     actionOnTokenFnType action = std::nullopt) {
        if (currHasType(kind)) {
            // TODO - Print in case of identifier

            // If there is an action passed then apply it
            if (action.has_value())
                action.value()(curr_token);
            nextToken();
            return true;
        } else {
            return false;
        }
    }

    // this will try to consume the next token if it can
    // otherwise it will throw runtime error
    // this should we called when we expect a certain kind of token to
    // be present. If the token is not present then its a syntactical error
    void consume(const lexer::TokenType &kind,
                 actionOnTokenFnType action = std::nullopt) {
        if (!try_consume(kind, action)) {
            std::stringstream errMsg;
            errMsg << "Expected token kind " << kind << " not found.";
            errMsg << " Curr Token: " << curr_token;
            abort(errMsg.str());
        }
    }

    void abort(std::string msg) {
        throw std::runtime_error("[PARSER]: " + msg);
    }

    // [IDENT]
    void intial_state_identifier() {
        consume(lexer::TokenType::LeftBracket);
        std::string state;
        auto storeState = [&](const lexer::Token &tok) { state = tok.token; };
        consume(lexer::TokenType::IDENT, storeState);
        tree.initial_state = state;
        tree.states.push_back(state);
        consume(lexer::TokenType::RightBracket);
    }

    // (IDENT COMMA)*
    bool try_ident_comma(actionOnTokenFnType action = std::nullopt) {
        auto ident = try_consume(lexer::TokenType::IDENT, action);
        if (!ident)
            return false;
        consume(lexer::TokenType::COMMA);
        return true;
    }

    bool try_comma_ident(actionOnTokenFnType action = std::nullopt) {
        auto comma = try_consume(lexer::TokenType::COMMA);
        if (!comma)
            return false;
        consume(lexer::TokenType::IDENT, action);
        return true;
    }

    //  ((IDENT COMMA)*)
    void try_state_ident_comma() {
        std::string state;
        auto storeState = [&](const lexer::Token &tok) { state = tok.token; };
        auto match = false;
        while (1) {
            match = try_ident_comma(storeState);
            if (match)
                tree.states.push_back(state);
            else
                break;
        }
    }

    //  ((COMMA IDENT)*)
    void try_state_comma_ident() {
        std::string state;
        auto storeState = [&](const lexer::Token &tok) { state = tok.token; };
        auto match = false;
        while (1) {
            match = try_comma_ident(storeState);
            if (match)
                tree.states.push_back(state);
            else
                break;
        }
    }

    // ((IDENT COMMA)*) (INITIAL_STATE) (COMMA IDENT)*
    void parse() {
        try_state_ident_comma();
        intial_state_identifier();
        // parse more
        try_state_comma_ident();
    }
};

} // namespace parser

#endif