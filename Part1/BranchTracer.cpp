#include "BranchTracer.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace llvm;

/**
 * reference:
 * https://llvm.org/docs/WritingAnLLVMPass.html 
 */ 

char BranchTracer::ID = 0;

bool BranchTracer::runOnModule(Module &M) 
{
    // branch dictionary: vector to map branch ID to source file and line number
    std::vector<std::pair<std::string, std::string>> branchDict;

    for (Function &F : M) 
    {
        for (BasicBlock &BB : F) 
        {
            for (Instruction &I : BB) 
            {
                if (BranchInst *BI = dyn_cast<BranchInst>(&I)) 
                {
                    if (BI -> isConditional())                                  // Conditional branch
                        addBranchInfo(&I, BI, &branchDict);
                }
            }
        }
    }

    writeToOutfile(&branchDict);

    return false; // module was not modified
}

void BranchTracer::writeToOutfile(std::vector<std::pair<std::string, std::string>> *branchDict)
{
    std::ofstream TraceFile;
    TraceFile.open("BranchPointerTrace.txt");
    if (!TraceFile.is_open()) 
    {
        errs() << "Error: Could not open trace file\n";
        return;
    }

       // output branchDict information
    for (const auto &entry : *branchDict) {
        TraceFile << entry.first << ": " << entry.second << "\n";
    }
    TraceFile.close();
}

void BranchTracer::addBranchInfo(Instruction *I, BranchInst *BI, std::vector<std::pair<std::string, std::string>> *branchDict)
{
    Instruction *instruction = dyn_cast<Instruction>(I);
    const DebugLoc &debugInfo = instruction -> getDebugLoc();

    if (debugInfo)
    {
        std::string fileName = debugInfo -> getFilename().str();
        std::string line = std::to_string(debugInfo -> getLine());

        for (unsigned i = 0; i < BI->getNumSuccessors(); ++i) 
        {
            std::stringstream target;
            target << fileName;

            BasicBlock *successor = BI -> getSuccessor(i);
            Instruction &branchI  = successor -> front();

            const DebugLoc &branchDebugInfo = branchI.getDebugLoc();
            std::string branchLine = std::to_string(branchDebugInfo -> getLine());

            target << ", " << line << ", " << branchLine;
                
            branchDict -> push_back(std::make_pair("br_" + line, target.str()));
        }

    }   
}

static RegisterPass<BranchTracer> X("branch-pointer-tracer", "Part1: Branch-Pointer-Tracer");
