#include "lexer.hpp"
// #include "llvm/ADT/StringRef.h"
// #include "llvm/Support/raw_ostream.h"
// #include <fstream>
// #include "utils.hpp"
#include <iostream>
#include <memory>
#include <ostream>
// #include <stdexcept>

int main() {
    std::string fileName = "tests/examples/simple2.sm";
    auto lexer = std::make_unique<lexer::Lexer>(fileName);

    while (auto token = lexer->get_token()) {

        // End of file
        if (token->kind == lexer::TokenType::EOF_TOKEN)
            break;

        std::cout << token.value() << std::endl;
    }

    return 0;
}