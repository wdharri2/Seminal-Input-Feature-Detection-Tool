#include "BranchTracer.h"
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

using namespace llvm;

/**
 * reference:
 * https://llvm.org/docs/WritingAnLLVMPass.html 
 */ 

char BranchTracer::ID = 0;
int id = 0;
int functionIndex = 0;

// Value* FuncionPointer

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
                    filename = debugInfo -> getFilename().str();            // get the filename
                }

                if (BranchInst *BI = dyn_cast<BranchInst>(&I))  // if the instruction is a branch instruction
                {
                    if ( BI -> isConditional() )
                        printExecutedBranchInfo(Context, BI, M);
                }

                if (CallInst *CI = dyn_cast<CallInst>(&I)) 
                    printFunctionPtr(Context, CI, F, M);
            }
        }
    }

    writeToOutfile(llvm::sys::path::filename(filename).str());
    return true; // module was modified
}

/**
 * writes to the ouptut file
 * output file name is "../output/filename_BranchDictionary.txt"
 * for input file example.c, the output is written to "../output/example.c_BranchDictionary.txt"
 * 
 * parameters:
 *      filename - the source file's name
 *      branchDict  - the branch dictionary to output
 */
void BranchTracer::writeToOutfile(std::string filename)
{
    std::string file = "../output/" + filename + "_BranchDictionary.txt";

    std::ofstream OutFile;
    OutFile.open(file);
    if (!OutFile.is_open()) 
    {
        errs() << "Error: Could not open trace file\n";
        return;
    }

    errs() << "writing to " + file + "\n";
       // output branchDict information
    for (const auto &entry : branchDict) {
        OutFile << entry.first << ": " << entry.second << "\n";
    }
    OutFile.close();
}

/**
 * adds a print statement for function pointers
 * in the format "*func_value"
 * only for indirect function calls (functions invoked via a function pointer)
 *
 * parameters:
 *      Context
 *      Calling instruction
 *      Function
 *      Module
 */
void BranchTracer::printFunctionPtr(LLVMContext &Context, CallInst *CI, Function &F, Module &M)
{
    Function* printfFunc = M.getFunction("printf");
    if (!printfFunc) {
        // If 'printf' function is not found, declare it.
        FunctionType* printfFuncType = FunctionType::get(Type::getInt32Ty(Context), true);
        printfFunc = Function::Create(printfFuncType, Function::ExternalLinkage, "printf", M);
        printfFunc -> setCallingConv(CallingConv::C);
    }

    if (Function *calledFunc = CI -> getCalledFunction())
    {}      // direct function call
    else    // indirect function call (via function pointer)
    {
        IRBuilder<> builder(CI);
        Value *formatStr = builder.CreateGlobalStringPtr("*func_%p\n");
        Constant *functionPointer = ConstantExpr::getBitCast(&F, builder.getInt8PtrTy());
        Value *funcPtrValue = builder.CreatePtrToInt(functionPointer, builder.getInt64Ty());

        std::vector<Value *> args;
        args.push_back(formatStr);
        args.push_back(funcPtrValue);
        builder.CreateCall(printfFunc, args);
    }
}


/**
 * adds print statements to branches
 * these branches print their id when executed
 * adds each branch to the branchDict dictionary
 * key: branch id number, value: filename, branch line number, target line number
 * 
 * parameters:
 *      Context
 *      Branch Instruction
 *      Module
 */
void BranchTracer::printExecutedBranchInfo(LLVMContext &Context, BranchInst *BI, Module &M)
{
    Function* printfFunc = M.getFunction("printf");
    if (!printfFunc) {
        // If 'printf' function is not found, declare it.
        FunctionType* printfFuncType = FunctionType::get(Type::getInt32Ty(Context), true);
        printfFunc = Function::Create(printfFuncType, Function::ExternalLinkage, "printf", M);
        printfFunc -> setCallingConv(CallingConv::C);
    }

    // Get the DebugLoc information from the branch instruction
    const DebugLoc &debugInfo = BI -> getDebugLoc();

    if ( debugInfo ) 
    {
        std::string filename = debugInfo -> getFilename().str();            // get the filename
        filename = llvm::sys::path::filename(filename).str();
        std::string line = std::to_string(debugInfo -> getLine());          // get the branch_statement_line
        
        for ( unsigned i = 0; i < BI -> getNumSuccessors(); i++ )           // for each successor (target) of the branching statement
        {
            BasicBlock *successor = BI -> getSuccessor(i);                  // get the current target
            Instruction &targetI  = successor -> front();                   // this is the first instruction of that target

            const DebugLoc &branchDebugInfo = targetI.getDebugLoc();        // get the debug info for the target instruction
            if (branchDebugInfo)
            {
                IRBuilder<> builder(&targetI);                              // create an IR builder for the target instruction
                Value *formatStr = builder.CreateGlobalStringPtr("br_%d: %s, %s\n");    // setup the string formatter

                std::string branchLine = std::to_string(branchDebugInfo -> getLine());  // get the line number of the target
                int thisId = branchDict.size();

                branchDict[ "br_" + std::to_string(thisId) ] = filename + ", " + line + ", " + branchLine;
                errs() << "br_" + std::to_string(thisId) << " " << filename << ", " << line << ", " << branchLine << "\n";

                Value *branchIdNum = builder.getInt32( thisId );
                Value *branchLineStr = builder.CreateGlobalStringPtr( line );
                Value *targetLineStr = builder.CreateGlobalStringPtr( branchLine );


                std::vector<Value *> args;
                args.push_back(formatStr);
                args.push_back(branchIdNum);      
                args.push_back(branchLineStr);
                args.push_back(targetLineStr);
                builder.CreateCall(printfFunc, args);
            }
        }
    }
}

// this registers the branch-pointer-tracer pass with the LLVM
static RegisterPass<BranchTracer> X("branch-pointer-tracer", "Part1: Branch-Pointer-Tracer");