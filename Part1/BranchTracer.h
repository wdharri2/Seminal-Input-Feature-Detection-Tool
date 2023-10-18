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
#include <vector>

namespace llvm 
{
    class BranchTracer : public ModulePass 
    {
        public:
            static char ID;
            BranchTracer() : ModulePass(ID) {}
            bool runOnModule(Module &M) override;

        private:
            void addBranchInfo(Instruction *I, BranchInst *BI, std::vector<std::pair<std::string, std::string>> *branchDict);
            void writeToOutfile(std::vector<std::pair<std::string, std::string>> *branchDict);
    };
}

#endif