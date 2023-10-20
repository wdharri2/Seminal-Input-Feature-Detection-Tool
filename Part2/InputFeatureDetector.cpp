#include "InputFeatureDetector.h"

// Pass ID variable  
char InputFeatureDetector::ID = 0;

// Run analysis on module
// This is almost identical to the part 1 code. TODO consolidate this and part 1 code
bool InputFeatureDetector::runOnModule(Module &M) {
    // branch dictionary: vector to map branch ID to source file and line number
    std::vector<std::pair<std::string, std::string>> branchDict;
    // source filename
    std::string filename;

    for (Function &F : M)               // iterate over all functions in the module
    {
        for (BasicBlock &BB : F)        // iterate over all basic blocks in the function
        {
            for (Instruction &I : BB)   // iterate over all instructions in the basic block
            {
                if (DILocation *DebugLoc = I.getDebugLoc())     // get the Debug Information for the instruction
                    filename = DebugLoc -> getFilename().str(); // use the Debug info to get the module filename

                if (BranchInst *BI = dyn_cast<BranchInst>(&I))  // if the instruction is a branch instruction
                {                                               // TODO: edit to work with all branch instructions
                    if (BI -> isConditional())                  // if the instruction is a conditional branch
                        detectFeatures(&I, BI, &branchDict);     // detect seminal input features and add the branch information to dictionary     
                }
            }
        }
    }

    // output out dictionary to the output file
    //writeToOutfile(llvm::sys::path::filename(filename).str(), &branchDict);
    return false; // module was not modified
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

// Detect input features influencing key points  
void InputFeatureDetector::detectFeatures(Instruction *Inst, BranchInst *branchInst, std::vector<std::pair<std::string, std::string>> *branchDict)
{

  // TODO:
  // Perform def-use analysis
  //    Determine input features affecting them
  // Append newfound info to branchDict

}