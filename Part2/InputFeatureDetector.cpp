#include "InputFeatureDetector.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "llvm/Support/Path.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

// Pass ID variable  
char InputFeatureDetector::ID = 0;

// Run analysis on module
// This is almost identical to the part 1 code. TODO consolidate this and part 1 code
bool InputFeatureDetector::runOnModule(Module &M) 
{
    LLVMContext& Context = M.getContext();
    std::string filename;

    for (Function &F : M)               // iterate over all functions in the module
    {
        for (BasicBlock &BB : F)        // iterate over all basic blocks in the function
        {
            for (Instruction &I : BB)   // iterate over all instructions in the basic block
            {
                if ( filename.empty())
                {
                    const DebugLoc &debugInfo = I.getDebugLoc();
                    filename = debugInfo -> getFilename().str();        // get the filename
                }

                if (BranchInst *BI = dyn_cast<BranchInst>(&I))          // if the instruction is a branch instruction
                    if ( BI -> isConditional() )                        // and a conditional branch
                        // printExecutedBranchInfo(Context, BI, M);
                        detectBranch(Context, BI, M);

                if (CallInst *CI = dyn_cast<CallInst>(&I))              // if the instruction is a call instruction
                    // printFunctionPtr(Context, CI, F, M);
                    detectCall(Context, CI, F, M);
            }
        }
    }

    writeToOutfile(llvm::sys::path::filename(filename).str());
    return true; // module was modified
}

/*
The output of the tool should indicate what the seminal input 
features are for the given program. For Example 2.1, the output 
can be as follows:
      Line 4: n

For Example 2.2, the output can be as follows:

      Line 3: size of file fp

The example output format can vary. What we showed above are just 
examples. Your document should explain the output format of your design.
*/
// Write output to file
void InputFeatureDetector::writeToOutfile(std::string filename, std::vector<std::pair<std::string, std::string>> *branchDict) {

  // TODO: Implement output writing
  
}

// Detect input features influencing key points for branch instructions 
void InputFeatureDetector::detectBranch(LLVMContext& Context, BranchInst *BI, Module &M)
{

    // print branch instruction to stdin for now to test
    err() << "Branch Instruction: " << *BI << "\n";
    std::cout << "Branch Instruction: " << *BI << "\n";
    //exit program
    exit(0);


}

// Detect input features influencing key points for call instructions
void InputFeatureDetector::detectCall(LLVMContext& Context, CallInst *CI, Function &F, Module &M)
{

}