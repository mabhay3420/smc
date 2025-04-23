#include "lexer.hpp"
#include "parser.hpp"
// #include "llvm/ADT/StringRef.h"
// #include "llvm/Support/raw_ostream.h"
#include <memory>
// #include <stdexcept>

int main() {
    std::string fileName = "tests/examples/test.sm";
    auto s = "a, t ,[b], c, d";
    auto lexer = std::make_unique<lexer::Lexer>(s, false);
    auto parser = std::make_unique<parser::Parser>(std::move(lexer));
    parser->parse();
    std::cout << "[" << parser->tree.initial_state << "]" << std::endl;
    // std::cout << parser.
    for (auto state : parser->tree.states)
        std::cout << state << std::endl;
    return 0;
}