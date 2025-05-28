#include "parser.hpp"
#include "lexer.hpp"
#include <optional>
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

void to_json(json &j, const TransitionStep &step) {
    std::visit([&j](auto &&arg) { to_json(j, arg); }, step);
}
void to_json(json &j, const Star &star) { j = "*"; }
void to_json(json &j, const OR &orCond) {
    j["OR"] = json::array();
    for (const auto &sym : orCond.sym) {
        j["OR"].push_back(sym);
    }
}
void to_json(json &j, const Condition &cond) {
    std::visit([&j](auto &&arg) { to_json(j, arg); }, cond);
}
void to_json(json &j, const R &r) { j = "R"; }
void to_json(json &j, const L &l) { j = "L"; }
void to_json(json &j, const X &x) { j = "X"; }
void to_json(json &j, const P &p) { j["P"] = p.sym; }
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