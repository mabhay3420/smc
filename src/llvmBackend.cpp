#include "llvmBackend.hpp"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/ir/IRBuilder.h"
#include "llvm/ir/LLVMContext.h"
#include "llvm/ir/Module.h"
#include <llvm/IR/DerivedTypes.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/TargetParser/Triple.h>
#include <ostream>
#include <sstream>
#include <string>

namespace llvmBackend {

void LllvmBackend::getIr() {
    llvm::LLVMContext context;
    llvm::Module module("smc", context);
    llvm::IRBuilder<> builder(context);
    llvm::Triple targetTriple("arm64-apple-macosx14.0.0");
    module.setTargetTriple(targetTriple);
    auto i32Type = builder.getInt32Ty();
    auto i8Type = builder.getInt8Ty();
    // default address space
    auto ptrType = builder.getPtrTy();
    auto mainFnType = llvm::FunctionType::get(i32Type, {}, false);
    auto printFnType = llvm::FunctionType::get(i32Type, {ptrType}, false);
    auto mainFn = llvm::Function::Create(
        mainFnType, llvm::Function::ExternalLinkage, "main", &module);
    auto printfFn = llvm::Function::Create(
        printFnType, llvm::Function::ExternalLinkage, "printf", &module);
    auto entryBlock = llvm::BasicBlock::Create(context, "entry", mainFn);
    builder.SetInsertPoint(entryBlock);
    auto printfPrompt =
        builder.CreateGlobalString("New print\n", "printfPrompt");
    auto retValue = builder.getInt32(0);
    builder.CreateCall(printfFn, {printfPrompt}); // Call printf with the string
    builder.CreateRet(retValue);

    // module.print(ss, nullptr);
    llvm::raw_string_ostream ss(ir);
    module.print(ss, nullptr);
    // ir = builder
    // Create a module in the context
    // auto moduel;
}

} // namespace llvmBackend