#include "lexer.hpp"
#include "parser.hpp"
#include <nlohmann/json.hpp>
// #include "llvm/ADT/StringRef.h"
// #include "llvm/Support/raw_ostream.h"
#include <memory>
// #include <stdexcept>
using json = nlohmann::json;
int main() {
    std::string fileName = "tests/examples/simple2.sm";
    auto lexer = std::make_unique<lexer::Lexer>(fileName);
    auto parser = std::make_unique<parser::Parser>(std::move(lexer));
    json ex3 = {
        {"happy", true},
        {"pi", 3.141},
    };
    std::cout << ex3.dump(4) << std::endl;
    parser->parse();
    parser::dump(parser->tree, std::cout);
    return 0;
}