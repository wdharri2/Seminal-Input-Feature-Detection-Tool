// InputFeatureDetector.h

// LLVM analysis pass to detect input features  
// influencing key control points in a module.

#ifndef INPUT_FEATURE_DETECTOR_H
#define INPUT_FEATURE_DETECTOR_H

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include <utility>
#include <string>
#include <vector>

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
    void writeOutput(std::string filename);

  private:

    // Analysis state
    std::set<Value*> InputFeatures;

    // Branch info from BranchTracer
    std::vector<std::pair<std::string, std::string>> BranchDict;

    // Detect input features
    void detectFeatures(Module *M);

  };

}  

#endif // INPUT_FEATURE_DETECTOR_H