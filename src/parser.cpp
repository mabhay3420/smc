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

// helper function to convert a range to a JSON array
template <class Range> auto to_json_array(const Range &r) {
    json arr = json::array();
    std::transform(r.begin(), r.end(), std::back_inserter(arr),
                   [](auto &&elem) {
                       json j;
                       to_json(j, elem);
                       return j;
                   });
    return arr;
}

void to_json(json &j, const TransitionStep &step) {
    std::visit([&j](auto &&arg) { to_json(j, arg); }, step);
}
void to_json(json &j, const Star &star) { j = "*"; }
void to_json(json &j, const OR &orCond) { j["OR"] = to_json_array(orCond.sym); }
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
    j["steps"] = to_json_array(tr.steps);
    j["finalState"] = tr.finalState;
}
void to_json(json &j, const ParseTree &tree) {
    j["initialState"] = tree.initial_state;
    j["states"] = tree.states;
    j["symbols"] = tree.symbols;
    j["transitions"] = to_json_array(tree.transitions);
}

} // namespace parser