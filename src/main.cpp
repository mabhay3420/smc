#include "lexer.hpp"
#include "llvmBackend.hpp"
#include "parser.hpp"
#include "utils.hpp"
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
    auto llvmBackend =
        std::make_unique<llvmBackend::LllvmBackend>(std::move(parser));
    nlohmann::json j;
    llvmBackend->dumpParseTree(j);
    dump_json_to_file("misc/example.json", j);
    llvmBackend->getIr();
    dump_string_to_file("misc/a.ll", llvmBackend->ir);
    // std::cout << "IR: " << llvmBackend->ir << std::endl;
    return 0;
}