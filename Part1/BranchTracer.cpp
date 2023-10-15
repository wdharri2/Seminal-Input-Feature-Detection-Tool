#include "llvm/BranchTracer.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
using namespace llvm;

char BranchTracer::ID = 0;

bool BranchTracer::runOnFunction(Function &F) 
{
    // branch dictionary: map branch ID to source file and line number
    std::unordered_map<std::string, std::string> branchDict;

    for (BasicBlock &BB : F) 
    {
        for (Instruction &I : BB) 
        {
            if (BranchInst *BI = dyn_cast<BranchInst>(&I)) 
            {
                if (BI -> isConditional())                                  // Conditional branch
                {
                    Value *Condition = BI -> getCondition();  // gets the condition
                } 
                else                                                        // Unconditional branch
                {

                }
            } 
            else if (SwitchInst *SI = dyn_cast<SwitchInst>(&I))             // Switch statement
            {

            } 
            else if (IndirectBrInst *IBI = dyn_cast<IndirectBrInst>(&I))    // Indirect branch
            {

            }
            // more branches?
        }
    }

    // TODO: output branchDict

    return false; // function was not modified
}

static RegisterPass<BranchTracer> X("branch-pointer-tracer", "Part1: Branch-Pointer-Tracer");
// to run with LLVM: 
// opt -load /Part1/BranchTracer.so -branch-pointer-tracer < input.ll > output.ll
