#include "lexer.hpp"
#include "llvmBackend.hpp"
#include "parser.hpp"
#include "utils.hpp"
#include <memory>
#include <nlohmann/json.hpp>
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
    return 0;
}