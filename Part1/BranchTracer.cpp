#include "llvm/BranchTracer.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include <iostream>
using namespace llvm;

char BranchTracer::ID = 0;

void addBranchInfo(Instruction *I, BranchInst *BI, std::unordered_map<std::string, std::string> *branchDict)
{
    // if there is metadata for the instruction (the branch is executed)
    if (MDNode *N = I -> getMetadata("dbg")) {
        DILocation Loc(N);  // DILocation - debugging information

        std::string fileName = Loc.getFileName().str();
        int line = Loc.getLineNumber();
        branchDictionary["br_" + std::to_string(BI -> getSuccessor(0) -> getName())] =
                    fileName + ", " + std::to_string(line);
        branchDictionary["br_" + std::to_string(BI -> getSuccessor(1) -> getName())] =
                    fileName + ", " + std::to_string(line);
    }
}

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
                    addBranchInfo(I, BI, branchDict);
                    // // if there is metadata for the instruction (the branch is executed)
                    // if (MDNode *N = I -> getMetadata("dbg")) {
                    //     DILocation Loc(N);  // DILocation - debugging information

                    //     std::string fileName = Loc.getFileName().str();
                    //     int line = Loc.getLineNumber();
                    //     branchDictionary["br_" + std::to_string(BI->getSuccessor(0)->getName())] =
                    //                 fileName + ", " + std::to_string(line);
                    //     branchDictionary["br_" + std::to_string(BI->getSuccessor(1)->getName())] =
                    //                 fileName + ", " + std::to_string(line);
                    // }


                    // Value *Condition = BI -> getCondition();  // gets the condition
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

    // output branchDict information
    for (auto &entry : branchDict) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }

    return false; // function was not modified
}

static RegisterPass<BranchTracer> X("branch-pointer-tracer", "Part1: Branch-Pointer-Tracer");
// to run with LLVM: 
// opt -load /Part1/BranchTracer.so -branch-pointer-tracer < input.ll > output.ll
