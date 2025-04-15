#include <iostream>
#include "lexer.hpp"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringRef.h"

int main() {
    llvm::StringRef message = "Hello LLVM!";
    std::cout << "hello world" << std::endl;
    llvm::outs() << message << "\n";
    return 0;
}