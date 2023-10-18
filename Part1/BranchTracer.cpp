#include "BranchTracer.h"
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
using namespace llvm;



/**
 * reference:
 * https://llvm.org/docs/WritingAnLLVMPass.html 
 */ 


char BranchTracer::ID = 0;

bool BranchTracer::runOnModule(Module &M) 
{
    // branch dictionary: map branch ID to source file and line number
    std::unordered_map<std::string, std::string> branchDict;

    for (Function &F : M) 
    {
        for (BasicBlock &BB : F) 
        {
            for (Instruction &I : BB) 
            {
                if (BranchInst *BI = dyn_cast<BranchInst>(&I)) 
                {
                    if (BI -> isConditional())                                  // Conditional branch
                    {
                        addBranchInfo(&I, BI, &branchDict);
                        // Value *Condition = BI -> getCondition();  // gets the condition
                    } 
                    else                                                        // Unconditional branch
                    {
                        // addBranchInfo(I, BI, branchDict);
                    }
                } 
                else if (SwitchInst *SI = dyn_cast<SwitchInst>(&I))             // Switch statement
                {
                    // addBranchInfo(I, BI, branchDict);
                } 
                else if (IndirectBrInst *IBI = dyn_cast<IndirectBrInst>(&I))    // Indirect branch
                {
                    // addBranchInfo(I, BI, branchDict);
                }
                // more branches?
            }
        }
    }

    writeToOutfile(&branchDict);

    return false; // function was not modified
}

void BranchTracer::writeToOutfile(std::unordered_map<std::string, std::string> *branchDict)
{
    std::ofstream TraceFile;
    TraceFile.open("BranchPointerTrace.txt");
    if (!TraceFile.is_open()) 
    {
        errs() << "Error: Could not open trace file\n";
        return;
    }

       // output branchDict information
    for (auto &entry : *branchDict) {
        TraceFile << entry.first << ": " << entry.second << "\n";
    }
    TraceFile.close();
}

void BranchTracer::addBranchInfo(Instruction *I, BranchInst *BI, std::unordered_map<std::string, std::string> *branchDict)
{
    Instruction *instruction = dyn_cast<Instruction>(I);
    const DebugLoc &debugInfo = instruction -> getDebugLoc();

    std::string fileName = debugInfo -> getFilename().str();
    int line = debugInfo -> getLine();

    std::string branch1 = BI -> getSuccessor(0) -> getName().str();
    (*branchDict)["br_" + branch1] = fileName + ", " + std::to_string(line);

    std::string branch2 = BI -> getSuccessor(1) -> getName().str();
    (*branchDict)["br_" + branch2] = fileName + ", " + std::to_string(line);
}

static RegisterPass<BranchTracer> X("branch-pointer-tracer", "Part1: Branch-Pointer-Tracer");
