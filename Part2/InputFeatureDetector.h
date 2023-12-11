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
#include "llvm/Analysis/LoopInfo.h"
#include <map>
#include <fstream>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <regex>
#include <iostream>


using namespace llvm;

namespace {

    class InputFeatureDetector : public ModulePass {

        public:
            // Pass identification
            static char ID;
            InputFeatureDetector() : ModulePass(ID) {}

            // void getAnalysisUsage(AnalysisUsage &AU) const override {
            //     AU.addRequired<LoopInfoWrapperPass>();
            // } 
            void getAnalysisUsage(AnalysisUsage &AU) const override;

            // Main analysis function
            bool runOnModule(Module &M) override; 

        private:
    
            // Analysis state
            std::set<Value*> InputFeatures;

            // Detect branch features
            void detectBranch(BranchInst *BI);

            // Detect call features
            void detectCall(LLVMContext& Context, CallInst *CI, Function &F, Module &M);

            void processOperand(Value *operand);

            Value* traceToSource(Instruction* inst);

            std::string getLineFromFile(int lineNumber, const std::string& filename);
            
            bool startsWithControlStructure(const std::string& line);

            std::string trim(const std::string& str);

            std::string extractVariableName(const std::string& line);
        };

}  

#endif // INPUT_FEATURE_DETECTOR_H