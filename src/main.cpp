#include "lexer.hpp"
#include "parser.hpp"
// #include "llvm/ADT/StringRef.h"
// #include "llvm/Support/raw_ostream.h"
#include <memory>
// #include <stdexcept>

int main() {
    std::string fileName = "tests/examples/simple2.sm";
    // auto s = "STATES: [a], [b]";
    auto lexer = std::make_unique<lexer::Lexer>(fileName);
    auto parser = std::make_unique<parser::Parser>(std::move(lexer));
    parser->parse();
    // std::cout << parser->tree << std::endl;
    parser::dump(parser->tree, std::cout);
    return 0;
}