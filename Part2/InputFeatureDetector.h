// InputFeatureDetector.h

// LLVM analysis pass to detect input features  
// influencing key control points in a module.

#ifndef INPUT_FEATURE_DETECTOR_H
#define INPUT_FEATURE_DETECTOR_H

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

using namespace llvm;

namespace {

    class InputFeatureDetector : public ModulePass {

        public:
            // Pass identification
            static char ID;
            InputFeatureDetector() : ModulePass(ID) {}

            // Main analysis function
            bool runOnModule(Module &M) override; 
            
            // Write output to file
            void writeToOutfile(std::string filename/*, std::vector<std::pair<std::string, std::string>> *branchDict*/);

        private:

            // Analysis state
            std::set<Value*> InputFeatures;

            // Branch info from BranchTracer
            std::vector<std::pair<std::string, std::string>> BranchDict;

            // Detect branch features
            void detectBranch(LLVMContext& Context, BranchInst *BI, Module &M);

            // Determine loop bound
            Value* determineLoopBound(LLVMContext &Context, BranchInst *BI, Module &M, Value *leftOperand, Value *rightOperand);

            // Determine loop counter
            bool isOperandLoopCounter(Loop *loop, Value *operand);

            // Detect call features
            void detectCall(LLVMContext& Context, CallInst *CI, Function &F, Module &M);

            void processOperand(Value *operand);

            Value* traceToSource(Instruction* inst);
        };

}  

#endif // INPUT_FEATURE_DETECTOR_H