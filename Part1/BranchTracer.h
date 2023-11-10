#ifndef LLVM_TRANSFORMS_BRANCH_TRACER_H
#define LLVM_TRANSFORMS_BRANCH_TRACER_H

#include "BranchTracer.h"
#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/PassRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include <map>

namespace llvm 
{
    class BranchTracer : public ModulePass 
    {
        public:
            static char ID;
            BranchTracer() : ModulePass(ID) {}
            bool runOnModule(Module &M) override;

        private:
            std::map<std::string, std::string> branchDict;

            void printFunctionPtr(LLVMContext &Context, CallInst *CI, Function &F, Module &M);
            void printExecutedBranchInfo(LLVMContext &Context, BranchInst *BI, Module &M);

            void addBranchInfo(Instruction *I, BranchInst *BI, std::vector<std::pair<std::string, std::string>> *branchDict);
            void writeToOutfile(std::string filename);
    };
}

#endif