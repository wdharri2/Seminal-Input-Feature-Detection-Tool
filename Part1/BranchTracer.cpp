#include "BranchTracer.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
using namespace llvm;



/**
 * reference:
 * https://llvm.org/docs/WritingAnLLVMPass.html 
 */ 

char BranchTracer::ID = 0;

bool BranchTracer::runOnModule(Module &M) 
{
    // branch dictionary: map branch ID to source file and line number
    std::map<std::string, std::string> branchDict;

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
                    } 
                    else                                                        // Unconditional branch
                    {
                        addBranchInfo(&I, BI, &branchDict);
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

void BranchTracer::writeToOutfile(std::map<std::string, std::string> *branchDict)
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

void BranchTracer::addBranchInfo(Instruction *I, BranchInst *BI, std::map<std::string, std::string> *branchDict)
{
    Instruction *instruction = dyn_cast<Instruction>(I);
    const DebugLoc &debugInfo = instruction -> getDebugLoc();

    if (debugInfo)
    {
        std::string fileName = debugInfo -> getFilename().str();
        int line = debugInfo -> getLine();

        std::stringstream targets;
        targets << fileName;

        for (unsigned i = 0; i < BI->getNumSuccessors(); ++i) 
        {
            BasicBlock *successor = BI -> getSuccessor(i);
            Instruction &branchI  = successor -> front();

            const DebugLoc &branchDebugInfo = branchI.getDebugLoc();
            int branchLine = branchDebugInfo -> getLine();

            targets << ", " << std::to_string(branchLine);
        }

        (*branchDict)["br_" + std::to_string(line)] = targets.str();
    }   
}

static RegisterPass<BranchTracer> X("branch-pointer-tracer", "Part1: Branch-Pointer-Tracer");
