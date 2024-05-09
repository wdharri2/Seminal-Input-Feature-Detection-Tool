/*
 * Copyright 2024 Willie D. Harris, Jr.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
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
#include <vector>


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

            std::vector<std::string> parseCondition(const std::string& str);

            std::string extractBetweenParentheses(const std::string& str);

            std::string tr(const std::string& str);
        };

}  

#endif // INPUT_FEATURE_DETECTOR_H