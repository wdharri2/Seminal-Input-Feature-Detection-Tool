#include "BranchTracer.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "llvm/Support/Path.h"
#include "llvm/Support/FileSystem.h"
// #include <llvm/Support/FileSystem.h>
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
    std::string filename;

    for (Function &F : M) 
    {
        for (BasicBlock &BB : F) 
        {
            for (Instruction &I : BB) 
            {
                if (DILocation *DebugLoc = I.getDebugLoc())
                    filename = DebugLoc -> getFilename().str();

                if (BranchInst *BI = dyn_cast<BranchInst>(&I)) 
                {
                    if (BI -> isConditional())                                  // Conditional branch
                        addBranchInfo(&I, BI, &branchDict);                        
                }
            }
        }
    }

    writeToOutfile(llvm::sys::path::filename(filename).str(), &branchDict);

    return false; // module was not modified
}

void BranchTracer::writeToOutfile(std::string filename, std::vector<std::pair<std::string, std::string>> *branchDict)
{
    std::string file = filename + "_BPT.txt";

    errs() << file << "\n";
    std::ofstream TraceFile;
    TraceFile.open(file);
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


int id = 0;
void BranchTracer::addBranchInfo(Instruction *I, BranchInst *BI, std::vector<std::pair<std::string, std::string>> *branchDict)
{
    Instruction *instruction = dyn_cast<Instruction>(I);
    const DebugLoc &debugInfo = instruction -> getDebugLoc();

    if (debugInfo)
    {
        std::string filename = debugInfo -> getFilename().str();
        std::string line = std::to_string(debugInfo -> getLine());

        for (unsigned i = 0; i < BI->getNumSuccessors(); ++i) 
        {
           // errs() <<  "\t" << i << "\n";
            std::stringstream target;
            target << llvm::sys::path::filename(filename).str();

            BasicBlock *successor = BI -> getSuccessor(i);
            Instruction &branchI  = successor -> front();

            const DebugLoc &branchDebugInfo = branchI.getDebugLoc();
            std::string branchLine = std::to_string(branchDebugInfo -> getLine());

            target << ", " << line << ", " << branchLine;
                
            branchDict -> push_back(std::make_pair("br_" + std::to_string(id++), target.str()));
        }
    }   
}

static RegisterPass<BranchTracer> X("branch-pointer-tracer", "Part1: Branch-Pointer-Tracer");
