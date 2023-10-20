#include "BranchTracer.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "llvm/Support/Path.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/IRBuilder.h"
using namespace llvm;

/**
 * reference:
 * https://llvm.org/docs/WritingAnLLVMPass.html 
 */ 

char BranchTracer::ID = 0;
int id = 0;


/**
 * runOnModule
 * overrides the ModulePass class' function
 * run on every module the LLVM runs on
 *
 * parameter: Module M
 * returns: true/false if the module was modified
 */
bool BranchTracer::runOnModule(Module &M) 
{
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
                        addBranchInfo(&I, BI, &branchDict);     // add the branch information to our dictinoary         
                }
            }
        }
    }

    // output out dictionary to the output file
    writeToOutfile(llvm::sys::path::filename(filename).str(), &branchDict);
    return false; // module was not modified
}

/**
 * writes to the ouptut file
 * output file name is "../output/filename_BPT.txt"
 * for input file example.c, the output is written to "../output/example.c_BPT.txt"
 * 
 * parameters:
 *      filename - the source file's name
 *      branchDict  - the branch dictionary to output
 */
void BranchTracer::writeToOutfile(std::string filename, std::vector<std::pair<std::string, std::string>> *branchDict)
{
    std::string file = "../output/" + filename + "_BPT.txt";

    std::ofstream OutFile;
    OutFile.open(file);
    if (!OutFile.is_open()) 
    {
        errs() << "Error: Could not open trace file\n";
        return;
    }

       // output branchDict information
    for (const auto &entry : *branchDict) {
        OutFile << entry.first << ": " << entry.second << "\n";
    }
    OutFile.close();
}


/**
 * addBranchInfo
 * adds the relevant branch information to the branch dictionary
 * in the form
 *      br_branchId : filename, branch_statement_line, branch_target_line
 *
 * for now, this is performed statically
 * by looking at all successors of a branch statement
 * and adding them to the dictionary
 *
 * parameters:
 *      I instruction - the instruction to add
 *      B branchInstruction - the branch instruction to add 
 *              (technically this refers to the same instruction as I)
 *              TODO: edit to only take one branch instruction as a parameter
 *      branchDict - the branch dictionary to add the branch information to
 */
void BranchTracer::addBranchInfo(Instruction *I, BranchInst *BI, std::vector<std::pair<std::string, std::string>> *branchDict)
{
    // get the debug info for the instruction ( this is how we get filename, line number )
    const DebugLoc &debugInfo = I -> getDebugLoc();

    // if there is debug info ( returns false if the source file wasn't compiled with -g, avoids seg fault )
    if (debugInfo)
    {
        std::string filename = debugInfo -> getFilename().str();            // get the filename
        std::string line = std::to_string(debugInfo -> getLine());          // get the branch_statement_line

        // iterate over the branch statements' successors
        // these are all of the possible targets of that branch statement
        // TODO: edit to only investigate the target that is actually tun
        for (unsigned i = 0; i < BI->getNumSuccessors(); ++i)
        {
            std::stringstream target;
            target << llvm::sys::path::filename(filename).str();            // add the filename to the target string

            BasicBlock *successor = BI -> getSuccessor(i);                  // get the current target
            Instruction &branchI  = successor -> front();                   // this is the first instruction of that target

            const DebugLoc &branchDebugInfo = branchI.getDebugLoc();        // get the debug info for the target instruction
            std::string branchLine = std::to_string(branchDebugInfo -> getLine());  // get the line number of the target

            target << ", " << line << ", " << branchLine;                   // prepare the value for this branch in the dictionary
                
            // add the br_id : filename, branch_statement_line, branch_target_line to the dictionary
            branchDict -> push_back(std::make_pair("br_" + std::to_string(id++), target.str()));
        }
    }   
}

// this registers the branch-pointer-tracer pass with the LLVM
static RegisterPass<BranchTracer> X("branch-pointer-tracer", "Part1: Branch-Pointer-Tracer");
