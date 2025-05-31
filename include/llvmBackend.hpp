#ifndef LLVM_BACKEND_HPP
#define LLVM_BACKEND_HPP 1
#include "parser.hpp"
#include <memory>
#include <string>

namespace llvmBackend {
class LllvmBackend {
  private:
    std::unique_ptr<parser::Parser> parser;

  public:
    std::string ir;
    LllvmBackend(std::unique_ptr<parser::Parser> inparser)
        : parser(std::move(inparser)) {
        parser->parse();
        ;
    };

    void dumpParseTree(nlohmann::json &j) { parser::to_json(j, parser->tree); }

    // Get IR
    void getIr();
};
} // namespace llvmBackend

#endif
