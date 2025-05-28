#include "parser.hpp"
#include "lexer.hpp"
#include "utils.hpp"
#include <optional>
#include <ostream>
#include <variant>

using json = nlohmann::json;

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

void dump(const TransitionStep &step, std::ostream &os, const Indent &indent) {
    std::visit([&os, &indent](auto &&arg) { dump(arg, os, indent); }, step);
}
void to_json(json &j, const TransitionStep &step) {
    std::visit([&j](auto &&arg) { to_json(j, arg); }, step);
}

void dump(const Star &star, std::ostream &os, const Indent &indent) {
    os << Line(indent, "*");
}
void to_json(json &j, const Star &star) { j = "*"; }
void dump(const OR &orCond, std::ostream &os, const Indent &indent) {
    std::string or_symbols;
    for (const auto &sym : orCond.sym) {
        or_symbols += sym + " ";
    }
    os << Line(indent, "OR { " + or_symbols + "}");
}
void to_json(json &j, const OR &orCond) {
    j["OR"] = json::array();
    for (const auto &sym : orCond.sym) {
        j["OR"].push_back(sym);
    }
}
void dump(const Condition &cond, std::ostream &os, const Indent &indent) {
    std::visit([&os, &indent](auto &&arg) { dump(arg, os, indent); }, cond);
}
void to_json(json &j, const Condition &cond) {
    std::visit([&j](auto &&arg) { to_json(j, arg); }, cond);
}
void to_json(json &j, const R &r) { j = "R"; }
void dump(const R &r, std::ostream &os, const Indent &indent) {
    os << Line(indent, "R");
}
void to_json(json &j, const L &l) { j = "L"; }
void dump(const L &l, std::ostream &os, const Indent &indent) {
    os << Line(indent, "L");
}
void to_json(json &j, const X &x) { j = "X"; }
void dump(const X &x, std::ostream &os, const Indent &indent) {
    os << Line(indent, "X");
}
void to_json(json &j, const P &p) { j = "P(" + p.sym + ")"; }
void dump(const P &p, std::ostream &os, const Indent &indent) {
    os << Line(indent, "P(" + p.sym + ")");
}
void dump(const Transition &tr, std::ostream &os, const Indent &indent) {
    os << Line(indent, "Transition {");
    os << Line(indent + 1, "Initial State: " + tr.initialState);
    os << Line(indent + 1, "Condition: {");
    parser::dump(tr.condition, os, indent + 2);
    os << Line(indent + 1, "}");
    os << Line(indent + 1, "Steps: {");
    for (const auto &step : tr.steps) {
        parser::dump(step, os, indent + 2);
    }
    os << Line(indent + 1, "}");
    os << Line(indent + 1, "Final State: " + tr.finalState);
    os << Line(indent, "}");
}

void to_json(json &j, const Transition &tr) {
    j["initialState"] = tr.initialState;
    to_json(j["condition"], tr.condition);
    j["steps"] = json::array();
    for (const auto &step : tr.steps) {
        json step_json;
        to_json(step_json, step);
        j["steps"].push_back(step_json);
    }
    j["finalState"] = tr.finalState;
}

void dump(const ParseTree &tree, std::ostream &os, const Indent &indent) {
    os << Line(indent, "ParseTree {");
    os << Line(indent + 1, "Initial State: " + tree.initial_state);
    os << Line(indent + 1, "States: {");
    for (const auto &state : tree.states) {
        os << Line(indent + 2, state);
    }
    os << Line(indent + 1, "}");
    os << Line(indent + 1, "Symbols: {");
    for (const auto &symbol : tree.symbols) {
        os << Line(indent + 2, symbol);
    }
    os << Line(indent + 1, "}");
    os << Line(indent + 1, "Transitions: {");
    for (const auto &tr : tree.transitions) {
        parser::dump(tr, os, indent + 2);
    }
    os << Line(indent + 1, "}");
    os << Line(indent, "}");
}

void to_json(json &j, const ParseTree &tree) {
    j["initialState"] = tree.initial_state;
    j["states"] = tree.states;
    j["symbols"] = tree.symbols;
    j["transitions"] = json::array();
    for (const auto &tr : tree.transitions) {
        json tr_json;
        to_json(tr_json, tr);
        j["transitions"].push_back(tr_json);
    }
}

} // namespace parser