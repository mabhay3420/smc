#include "lexer.hpp"
// #include "llvm/ADT/StringRef.h"
// #include "llvm/Support/raw_ostream.h"
// #include <fstream>
// #include "utils.hpp"
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <strstream>
// #include <stdexcept>

int main() {
    std::string fileName = "tests/examples/minimal1.sm";
    auto lexer = std::make_unique<lexer::Lexer>(fileName);
    while (1) {
        auto token = lexer->get_token();
        if (!token) {
            std::stringstream currLocation;
            currLocation << lexer->get_cursor();
            throw std::runtime_error("Unknown token at " + currLocation.str());
        }
        // End of file
        if (token->kind == lexer::TokenType::EOF_TOKEN)
            break;
        std::cout << token.value() << std::endl;
    }

    return 0;
}