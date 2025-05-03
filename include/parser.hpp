#ifndef PARSER_HPP
#define PARSER_HPP
#include "lexer.hpp"
#include <algorithm>
#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <ostream>
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

std::ostream &operator<<(std::ostream &os, const Star &star);
std::ostream &operator<<(std::ostream &os, const OR &orCond);

using Condition = std::variant<Star, OR>;

std::ostream &operator<<(std::ostream &os, const Condition &cond);

struct R {};
struct L {};
struct X {};
struct P {
    std::string sym;
};
std::ostream &operator<<(std::ostream &os, const R &r);
std::ostream &operator<<(std::ostream &os, const L &l);
std::ostream &operator<<(std::ostream &os, const X &x);
std::ostream &operator<<(std::ostream &os, const P &p);

using TransitionStep = std::variant<R, L, X, P>;

std::ostream &operator<<(std::ostream &os, const TransitionStep &step);

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

std::ostream &operator<<(std::ostream &os, const Transition &tr);

class ParseTree {
  public:
    std::vector<std::string> states;
    std::string initial_state;
    std::vector<std::string> symbols;
    std::vector<Transition> transitions;
};

std::ostream &operator<<(std::ostream &os, const ParseTree &tree);

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

    // LEFT_BRACKET IDENT RIGHT_BRACKET
    void intial_state_identifier() {
        consume(lexer::TokenType::LeftBracket);
        std::string state;
        auto storeState = [&](const lexer::Token &tok) { state = tok.token; };
        consume(lexer::TokenType::IDENT, storeState);
        tree.initial_state = state;
        tree.states.push_back(state);
        consume(lexer::TokenType::RightBracket);
    }

    //  ((IDENT COMMA)*)
    void try_state_ident_comma() {
        std::string state;
        auto storeState = [&](const lexer::Token &tok) { state = tok.token; };
        while (try_consume(lexer::TokenType::IDENT, storeState)) {
            tree.states.push_back(state);
            consume(lexer::TokenType::COMMA);
        }
    }

    //  ((COMMA IDENT)*)
    void try_state_comma_ident() {
        std::string state;
        auto storeState = [&](const lexer::Token &tok) { state = tok.token; };
        while (try_consume(lexer::TokenType::COMMA)) {
            consume(lexer::TokenType::IDENT, storeState);
            tree.states.push_back(state);
        }
    }

    // := ((IDENT COMMA)*) (`INITIAL_STATE`) (COMMA IDENT)*
    void state_list() {
        try_state_ident_comma();
        intial_state_identifier();
        try_state_comma_ident();
    }

    // := STATES COLON `STATE_LIST`
    void state_declaration() {
        consume(lexer::TokenType::STATES);
        consume(lexer::TokenType::COLON);
        state_list();
    }

    void skip_newlines() {
        while (try_consume(lexer::TokenType::NEWLINE)) {
            continue;
        }
    }

    // IDENT (COMMA IDENT)*
    void symbols_list() {
        std::string symbol;
        auto storeSymbol = [&](const lexer::Token &tok) { symbol = tok.token; };

        consume(lexer::TokenType::IDENT, storeSymbol);
        tree.symbols.push_back(symbol);

        while (try_consume(lexer::TokenType::COMMA)) {
            consume(lexer::TokenType::IDENT, storeSymbol);
            tree.symbols.push_back(symbol);
        }
    }
    /*────────────────────────────  GRAMMAR HELPERS *
     * ──────────────────────────────*/

    /* SYMBOL_DECLARATION := "SYMBOLS" ":" SYMBOL_LIST */
    void symbol_declaration() {
        consume(lexer::TokenType::SYMBOLS);
        consume(lexer::TokenType::COLON);
        symbols_list();
    }

    // IDENT OR X ( symbol for empty tape)
    Condition parse_or_conditions() {
        std::vector<std::string> syms;
        std::string sym;
        auto storeSym = [&](const lexer::Token &tok) { sym = tok.token; };

        if (curr_token.kind == lexer::TokenType::IDENT ||
            curr_token.kind == lexer::TokenType::X) {
            consume(curr_token.kind, storeSym);
        } else {
            abort("Expected IDENT or X token");
        }
        syms.push_back(sym);

        while (try_consume(lexer::TokenType::OR)) {
            if (curr_token.kind == lexer::TokenType::IDENT ||
                curr_token.kind == lexer::TokenType::X) {
                consume(curr_token.kind, storeSym);
            } else {
                abort("Expected IDENT or X token");
            }
            syms.push_back(sym);
        }
        return OR{syms};
    }

    /* CONDITION := STAR | OR_CONDITIONS */
    Condition parse_condition() {
        //  STAR
        if (try_consume(lexer::TokenType::STAR))
            return Star{};

        //  OR_CONDITIONS := (SYMBOL OR)* SYMBOL
        return parse_or_conditions();
    }

    /* ACTION := R | L | X | PRINT */
    TransitionStep parse_action() {
        const auto tok = curr_token;

        //  Simple one‑letter actions
        if (tok.kind == lexer::TokenType::R ||
            tok.kind == lexer::TokenType::L ||
            tok.kind == lexer::TokenType::X) {
            consume(tok.kind);                     // consume R/L/X
            return fromTokenAndValue(tok).value(); // safe: kind already matched
        }

        //  PRINT := P '(' SYMBOL ')'
        if (tok.kind == lexer::TokenType::P) {
            consume(lexer::TokenType::P);         // P
            consume(lexer::TokenType::LeftParen); // (
            std::string sym;
            auto storeSym = [&](const lexer::Token &t) { sym = t.token; };
            if (curr_token.kind == lexer::TokenType::IDENT ||
                curr_token.kind == lexer::TokenType::X) {
                consume(curr_token.kind, storeSym); // SYMBOL
            } else {
                abort("Expected IDENT or X token");
            }
            consume(lexer::TokenType::RightParen); // )
            return P{sym};
        }

        abort("Unknown action token: " + tok.token);
        return R(); // Unreachable, satisfies compiler
    }

    /* ACTION_LIST := (ACTION DASH)* ACTION */
    std::vector<TransitionStep> parse_action_list() {
        std::vector<TransitionStep> steps;
        steps.push_back(parse_action());

        while (try_consume(lexer::TokenType::DASH)) {
            steps.push_back(parse_action());
        }
        return steps;
    }

    /* TRANSITION := MATCH_STATE CONDITION ',' ACTION_LIST ',' FINAL_STATE */
    Transition parse_transition() {
        Transition tr;

        //  MATCH_STATE
        auto storeState = [&](const lexer::Token &tok) {
            tr.initialState = tok.token;
        };
        consume(lexer::TokenType::IDENT, storeState);
        consume(lexer::TokenType::COMMA);

        //  CONDITION
        tr.condition = parse_condition();

        consume(lexer::TokenType::COMMA);

        //  ACTION_LIST
        tr.steps = parse_action_list();

        consume(lexer::TokenType::COMMA);

        //  FINAL_STATE
        auto storeFinal = [&](const lexer::Token &tok) {
            tr.finalState = tok.token;
        };
        consume(lexer::TokenType::IDENT, storeFinal);

        return tr;
    }

    /* TRANSITION_LIST := (TRANSITION NEWLINE)* */
    void transition_list() {
        while (true) {
            // A transition must start with an IDENT ‑ anything else ends the
            // list.
            if (!currHasType(lexer::TokenType::IDENT))
                break;

            tree.transitions.push_back(parse_transition());
            // Each transition line ends with at least one NEWLINE
            skip_newlines();
        }
    }

    /* TRANSITION_DECLARATION := "TRANSITIONS" ':' NEWLINE TRANSITION_LIST */
    void transition_declaration() {
        consume(lexer::TokenType::TRANSITIONS);
        consume(lexer::TokenType::COLON);
        consume(lexer::TokenType::NEWLINE);
        skip_newlines(); // allow blank lines before list
        transition_list();
    }

    /*──────────────────────────────  TOP‑LEVEL *
     * ──────────────────────────────────*/
    /*  PROGRAM := STATE_DECLARATION NEWLINE SYMBOL_DECLARATION NEWLINE
     TRANSITION_DECLARATION */
    void parse() {
        skip_newlines(); // tolerate leading blank lines

        state_declaration();
        consume(lexer::TokenType::NEWLINE);
        skip_newlines();

        symbol_declaration();
        consume(lexer::TokenType::NEWLINE);
        skip_newlines();

        transition_declaration();
        skip_newlines();

        // Program should end here
        consume(lexer::TokenType::EOF_TOKEN);
    }
};

} // namespace parser

#endif