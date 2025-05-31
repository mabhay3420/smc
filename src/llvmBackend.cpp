#include "utils.hpp"
#include <iostream>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FormatVariadic.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Triple.h>
#include <llvm/ir/IRBuilder.h>
#include <llvm/ir/LLVMContext.h>
#include <llvm/ir/Module.h>
#include <llvmBackend.hpp>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <variant>

namespace llvmBackend {
using llvm::BasicBlock;
using llvm::Function;
using llvm::FunctionType;
using llvm::GlobalValue;
using llvm::IRBuilder;
using llvm::LLVMContext;
using llvm::Module;
using llvm::PointerType;
using llvm::Triple;
using llvm::Type;
using llvm::Value;
using parser::Condition;
using parser::Transition;
using parser::TransitionStep;
/// Convenience alias for brevity.
using BV = llvm::Value *;

/// Wrap `printf` var-args building because the C++ API is slightly noisier
/// than the Inkwell helper in your Rust code.
static void buildPrintf(IRBuilder<> &B, Function *printfFn, llvm::StringRef fmt,
                        llvm::ArrayRef<BV> extraArgs = {}) {
    auto *fmtPtr = B.CreateGlobalString(fmt, "fmt");
    std::vector<BV> args;
    args.reserve(1 + extraArgs.size());
    args.push_back(fmtPtr);
    args.insert(args.end(), extraArgs.begin(), extraArgs.end());
    B.CreateCall(printfFn, args);
}

void LllvmBackend::getIr() {
    LLVMContext ctx;
    Module mod("tape_machine_fixed", ctx);
    IRBuilder<> B(ctx);

    Triple triple("arm64-apple-macosx13.0.0");
    mod.setTargetTriple(triple);

    auto *i32 = B.getInt32Ty();
    auto *i8 = B.getInt8Ty();
    auto *i8Ptr = B.getPtrTy();

    //  extern int printf(char*, ...);
    auto *printfTy = FunctionType::get(i32, {i8Ptr}, /*isVarArg*/ true);
    auto *printfFn =
        Function::Create(printfTy, Function::ExternalLinkage, "printf", mod);

    //  extern void* malloc(int);
    auto *mallocTy = FunctionType::get(i8Ptr, {i32}, false);
    auto *mallocFn =
        Function::Create(mallocTy, Function::ExternalLinkage, "malloc", mod);

    //  extern int scanf(char*, ...);
    auto *scanfTy = FunctionType::get(i32, {i8Ptr}, /*isVarArg*/ true);
    auto *scanfFn =
        Function::Create(scanfTy, Function::ExternalLinkage, "scanf", mod);

    //  int main()
    auto *mainTy = FunctionType::get(i32, {}, false);
    auto *mainFn =
        Function::Create(mainTy, Function::ExternalLinkage, "main", mod);
    BasicBlock *entry = BasicBlock::Create(ctx, "entry", mainFn);
    B.SetInsertPoint(entry);

    // local allocas
    auto *numStepsPtr = B.CreateAlloca(i32, nullptr, "num_steps_ptr");
    auto *arrSizePtr = B.CreateAlloca(i32, nullptr, "arr_size_ptr");
    auto *currTapeIdx = B.CreateAlloca(i32, nullptr, "current_tape_index_ptr");
    auto *currStepPtr = B.CreateAlloca(i32, nullptr, "current_step_ptr");
    auto *currSymPtr = B.CreateAlloca(i32, nullptr, "current_symbol_index_ptr");
    auto *currStatePtr =
        B.CreateAlloca(i32, nullptr, "current_state_index_ptr");

    // Initialize all allocas to zero
    B.CreateStore(llvm::ConstantInt::get(i32, 0), currTapeIdx);
    B.CreateStore(llvm::ConstantInt::get(i32, 0), currStepPtr);
    B.CreateStore(llvm::ConstantInt::get(i32, 0), currSymPtr);
    B.CreateStore(llvm::ConstantInt::get(i32, 0), currStatePtr);

    // ask user for num-steps & tape-size
    buildPrintf(B, printfFn, "Enter number of steps: ");
    auto *scanfFmt = B.CreateGlobalString("%d", "scanf_fmt");
    B.CreateCall(scanfFn, {scanfFmt, numStepsPtr});

    buildPrintf(B, printfFn, "Enter array size: ");
    B.CreateCall(scanfFn, {scanfFmt, arrSizePtr});

    // malloc tape
    auto *arrSize = B.CreateLoad(i32, arrSizePtr, "arr_size");
    auto *tapePtr = B.CreateCall(mallocFn, {arrSize}, "tape_malloc");

    auto symbols = parser->tree.symbols;
    symbols.push_back("X"); // "X" is always last
    auto states = parser->tree.states;
    auto initialState = parser->tree.initial_state;
    auto transitions = parser->tree.transitions;

    // const-maps symbol/state → index
    std::unordered_map<std::string, unsigned> sym2idx;
    for (unsigned i = 0; i < symbols.size(); ++i)
        sym2idx[symbols[i]] = i;

    std::unordered_map<std::string, unsigned> state2idx;
    for (unsigned i = 0; i < states.size(); ++i)
        state2idx[states[i]] = i;

    const unsigned totalSyms = symbols.size();
    const unsigned totalStates = states.size();

    // print helpful legend
    {
        std::string mapping;
        for (unsigned i = 0; i < totalSyms; ++i) {
            std::stringstream ss;
            ss << i << ":" << symbols[i];
            mapping += ss.str();
            if (i + 1 != totalSyms)
                mapping += ", ";
        }
        buildPrintf(B, printfFn, "All Symbols: %s\n",
                    {B.CreateGlobalString(mapping)});
    }
    {
        std::string mapping;
        for (unsigned i = 0; i < totalStates; ++i) {
            std::stringstream ss;
            ss << i << ":" << states[i];
            mapping += ss.str();
            if (i + 1 != totalStates)
                mapping += ", ";
        }
        buildPrintf(B, printfFn, "All States: %s\n",
                    {B.CreateGlobalString(mapping)});
    }

    //  2 : fill tape with initial 'X'
    unsigned xIdx = sym2idx.at("X");
    BasicBlock *tapeInit = BasicBlock::Create(ctx, "tape_fill_init", mainFn);
    BasicBlock *tapeBody =
        BasicBlock::Create(ctx, "tape_fill_loop_body", mainFn);
    BasicBlock *tapeDone =
        BasicBlock::Create(ctx, "tape_fill_loop_end", mainFn);
    B.CreateBr(tapeInit);

    // tape_fill_init:
    B.SetInsertPoint(tapeInit);
    {
        auto *idx = B.CreateLoad(i32, currTapeIdx);
        auto *cond = B.CreateICmpULT(idx, arrSize, "loop_cond");
        B.CreateCondBr(cond, tapeBody, tapeDone);
    }

    // tape_fill_loop_body:
    B.SetInsertPoint(tapeBody);
    {
        auto *idx = B.CreateLoad(i32, currTapeIdx);
        auto *gep = B.CreateGEP(i32, tapePtr, {idx}, "tape_gep");
        B.CreateStore(llvm::ConstantInt::get(i32, xIdx), gep);

        auto *inc = B.CreateAdd(idx, llvm::ConstantInt::get(i32, 1));
        B.CreateStore(inc, currTapeIdx);
        B.CreateBr(tapeInit);
    }

    // tape_fill_loop_end:
    B.SetInsertPoint(tapeDone);
    B.CreateStore(llvm::ConstantInt::get(i32, 0), currTapeIdx);

    //  3 : debugging print of tape contents
    BasicBlock *pLoop = BasicBlock::Create(ctx, "print_loop", mainFn);
    BasicBlock *pBody = BasicBlock::Create(ctx, "print_body", mainFn);
    BasicBlock *pEnd = BasicBlock::Create(ctx, "print_end", mainFn);
    B.CreateBr(pLoop);

    B.SetInsertPoint(pLoop);
    {
        auto *idx = B.CreateLoad(i32, currTapeIdx);
        auto *cond = B.CreateICmpULT(idx, arrSize);
        B.CreateCondBr(cond, pBody, pEnd);
    }
    B.SetInsertPoint(pBody);
    {
        auto *idx = B.CreateLoad(i32, currTapeIdx);
        auto *gep = B.CreateGEP(i32, tapePtr, {idx});
        auto *val = B.CreateLoad(i32, gep);
        buildPrintf(B, printfFn, "Tape Content: %d\n", {val});
        auto *inc = B.CreateAdd(idx, llvm::ConstantInt::get(i32, 1));
        B.CreateStore(inc, currTapeIdx);
        B.CreateBr(pLoop);
    }
    B.SetInsertPoint(pEnd);
    B.CreateStore(llvm::ConstantInt::get(i32, 0), currTapeIdx);

    //  4 : main steps-loop (state-machine core) – identical to Rust version
    BasicBlock *stepsLoop = BasicBlock::Create(ctx, "steps_loop", mainFn);
    BasicBlock *stepsBody = BasicBlock::Create(ctx, "steps_loop_body", mainFn);
    BasicBlock *stepsExit = BasicBlock::Create(ctx, "steps_loop_end", mainFn);
    BasicBlock *afterSwitch = BasicBlock::Create(ctx, "after_switch", mainFn);
    B.CreateBr(stepsLoop);

    // steps_loop:
    B.SetInsertPoint(stepsLoop);
    {
        auto *step = B.CreateLoad(i32, currStepPtr);
        auto *n = B.CreateLoad(i32, numStepsPtr);
        auto *ok = B.CreateICmpULT(step, n, "step_limit_cond");
        B.CreateCondBr(ok, stepsBody, stepsExit);
    }

    // steps_loop_body:
    B.SetInsertPoint(stepsBody);

    auto *cStep = B.CreateLoad(i32, currStepPtr);
    buildPrintf(B, printfFn, "Current step: %d\n", {cStep});

    auto *cIdx = B.CreateLoad(i32, currTapeIdx);
    buildPrintf(B, printfFn, "Current tape index: %d\n", {cIdx});

    // switch dispatch  (symIdx * totalStates + stateIdx)
    auto *symIdx = B.CreateLoad(i32, currSymPtr);
    auto *stateIdx = B.CreateLoad(i32, currStatePtr);
    auto *lhsMul =
        B.CreateMul(symIdx, llvm::ConstantInt::get(i32, totalStates));
    auto *caseNum = B.CreateAdd(lhsMul, stateIdx, "switch_case");

    BasicBlock *switchDefault =
        BasicBlock::Create(ctx, "switch_default", mainFn);
    llvm::SwitchInst *sw =
        B.CreateSwitch(caseNum, switchDefault, totalSyms * totalStates);

    // ----- generate N×M basic-blocks
    std::vector<GlobalValue *> symStrings(totalSyms), stateStrings(totalStates);
    for (unsigned i = 0; i < totalSyms; ++i)
        symStrings[i] = B.CreateGlobalString(symbols[i], "sym_" + symbols[i]);
    for (unsigned i = 0; i < totalStates; ++i)
        stateStrings[i] = B.CreateGlobalString(states[i], "state_" + states[i]);

    std::vector<BasicBlock *> caseBlocks(totalSyms * totalStates);
    for (unsigned s = 0; s < totalSyms; ++s) {
        for (unsigned q = 0; q < totalStates; ++q) {
            unsigned num = s * totalStates + q;
            std::stringstream ss;
            ss << "state_" << states[q] << "_sym_" << symbols[s];
            caseBlocks[num] = BasicBlock::Create(ctx, ss.str(), mainFn);
            sw->addCase(llvm::ConstantInt::get(i32, num), caseBlocks[num]);
        }
    }

    // emit prints + fallthrough to afterSwitch for every case
    for (unsigned num = 0; num < caseBlocks.size(); ++num) {
        B.SetInsertPoint(caseBlocks[num]);
        unsigned s = num / totalStates;
        unsigned q = num % totalStates;
        buildPrintf(B, printfFn, "Symbol: %s State: %s\n",
                    {symStrings[s], stateStrings[q]});
        B.CreateBr(afterSwitch);
    }

    // “Star beats OR” and duplicate-case
    // suppression.
    std::unordered_set<unsigned> alreadyDone;
    auto sortedTransition = transitions;
    std::stable_sort(sortedTransition.begin(), sortedTransition.end(),
                     [](const Transition &A, const Transition &B) {
                         if (std::holds_alternative<parser::OR>(A.condition) &&
                             std::holds_alternative<parser::Star>(B.condition))
                             return true;
                         return false;
                     });
    for (const auto &T : sortedTransition) {

        // Visitor function for variant
        auto getSybols = overloaded{
            [&](const parser::Star &) -> const std::vector<std::string> & {
                return symbols;
            },
            [](const parser::OR &orCond) -> const std::vector<std::string> & {
                return orCond.sym;
            },
        };
        const std::vector<std::string> &symList =
            std::visit(getSybols, T.condition);

        for (const std::string &sym : symList) {
            unsigned s = sym2idx.at(sym);
            unsigned q = state2idx.at(T.initialState);
            unsigned caseNo = s * totalStates + q;
            if (!alreadyDone.insert(caseNo).second)
                continue; // already patched

            // splice before unconditional branch inside caseBlocks[caseNo]
            B.SetInsertPoint(caseBlocks[caseNo]->getTerminator());

            auto TransitionAction = overloaded{
                [&](const parser::L) {
                    auto *idx = B.CreateLoad(i32, currTapeIdx);

                    auto *idx_new =
                        B.CreateSub(idx, llvm::ConstantInt::get(i32, 1));
                    B.CreateStore(idx_new, currTapeIdx);
                },
                [&](const parser::R) {
                    auto *idx = B.CreateLoad(i32, currTapeIdx);

                    auto *idx_new =
                        B.CreateAdd(idx, llvm::ConstantInt::get(i32, 1));
                    B.CreateStore(idx_new, currTapeIdx);
                },
                [&](const parser::X) {
                    // noop
                },
                [&](const parser::P &p) {
                    unsigned pIdx = sym2idx.at(p.sym);
                    auto *idx = B.CreateLoad(i32, currTapeIdx);
                    auto *gep = B.CreateGEP(i32, tapePtr, {idx});
                    B.CreateStore(llvm::ConstantInt::get(i32, pIdx), gep);
                },
            };
            for (TransitionStep st : T.steps) {
                std::visit(TransitionAction, st);
            }
        } // for sym
    }     // for transitions

    // switch_default:
    B.SetInsertPoint(switchDefault);
    buildPrintf(B, printfFn, "Default Remainder: %d\n", {cStep});
    B.CreateBr(afterSwitch);

    // after_switch:
    B.SetInsertPoint(afterSwitch);
    auto *nextStep = B.CreateAdd(cStep, llvm::ConstantInt::get(i32, 1));
    B.CreateStore(nextStep, currStepPtr);

    // crude symbol-index update (exactly the same arithmetic as Rust stub)
    auto *nextSym = B.CreateAdd(symIdx, llvm::ConstantInt::get(i32, 2));
    nextSym = B.CreateSRem(nextSym, llvm::ConstantInt::get(i32, totalSyms));
    B.CreateStore(nextSym, currSymPtr);
    B.CreateBr(stepsLoop);

    // steps_loop_end:
    B.SetInsertPoint(stepsExit);
    buildPrintf(B, printfFn, "Reached end of steps loop.\n");
    B.CreateRet(llvm::ConstantInt::get(i32, 0));

    // ----- verify & return IR-string
    // ----------------------------------------
    if (llvm::verifyModule(mod, &llvm::errs()))
        throw std::runtime_error("generated module is invalid!");
    llvm::raw_string_ostream os(ir);
    mod.print(os, nullptr);
}
} // namespace llvmBackend