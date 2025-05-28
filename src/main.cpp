#include "lexer.hpp"
#include "parser.hpp"
#include <fstream>
#include <iostream>
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
    json j;
    parser::to_json(j, parser->tree);
    auto example_json_file = "misc/example.json";
    std::ofstream out(example_json_file);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to open output file: " +
                                 std::string(example_json_file));
    }
    out << j.dump(4); // Write JSON to file with 4 spaces indentation
    // std::cout << j.dump(2) << std::endl;
    return 0;
}