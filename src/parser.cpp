#include "parser.hpp"
#include "lexer.hpp"
#include "utils.hpp"
#include <optional>
#include <ostream>
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

std::ostream &operator<<(std::ostream &os, const TransitionStep &step) {
    std::visit([&os](auto &&arg) { os << arg; }, step);
    return os;
}

std::ostream &operator<<(std::ostream &os, const Star &star) {
    os << "*(ALWAYS)";
    return os;
}
std::ostream &operator<<(std::ostream &os, const OR &orCond) {
    os << "OR {";
    for (const auto &sym : orCond.sym) {
        os << sym << " ";
    }
    os << "}";
    return os;
}
std::ostream &operator<<(std::ostream &os, const Condition &cond) {
    std::visit([&os](auto &&arg) { os << arg; }, cond);
    return os;
}
std::ostream &operator<<(std::ostream &os, const R &r) {
    os << "R";
    return os;
}
std::ostream &operator<<(std::ostream &os, const L &l) {
    os << "L";
    return os;
}
std::ostream &operator<<(std::ostream &os, const X &x) {
    os << "X";
    return os;
}
std::ostream &operator<<(std::ostream &os, const P &p) {
    os << "P(" + p.sym + ")";
    return os;
}

/*
Transition {
    Initial State: "A"
    Conditions {
       OR { "B" "C" }
    }
    Steps: [R, L, P("X")],
    Final State: "D"
}
*/
std::ostream &operator<<(std::ostream &os, const Transition &tr) {
    Indent indent{0};
    os << Line(indent, "Transition {");
    indent += 1;
    os << Line(indent, "Initial State: " + tr.initialState);
    os << Line(indent, "Condition: {");
    indent += 1;
    // FIXME
    os << tr.condition;
    indent -= 1;
    os << Line(indent, "}");
    os << Line(indent, "Steps: {");
    indent += 1;
    // TODO - Fixme
    for (const auto &step : tr.steps) {
        os << step << ", ";
    }
    indent -= 1;
    os << Line(indent, "}");
    os << Line(indent, "Final State: " + tr.finalState);
    indent -= 1;
    os << Line(indent, "}");
    return os;
}

std::ostream &operator<<(std::ostream &os, const ParseTree &tree) {
    Indent indent{0};
    os << Line(indent, "ParseTree {");
    indent += 1;
    os << Line(indent, "Initial State: " + tree.initial_state);
    os << Line(indent, "States: {");
    indent += 1;
    for (const auto &state : tree.states) {
        os << Line(indent, state);
    }
    indent -= 1;
    os << Line(indent, "}");
    os << Line(indent, "Symbols: {");
    indent += 1;
    for (const auto &symbol : tree.symbols) {
        os << Line(indent, symbol);
    }
    indent -= 1;
    os << Line(indent, "}");
    os << Line(indent, "Transitions: {");
    indent += 1;
    // TODO - Fixme
    for (const auto &tr : tree.transitions) {
        os << tr;
    }
    indent -= 1;
    os << Line(indent, "}");
    indent -= 1;
    return os;
}

} // namespace parser