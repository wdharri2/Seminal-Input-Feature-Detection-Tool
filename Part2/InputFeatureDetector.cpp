#include "InputFeatureDetector.h"
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
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/InstIterator.h"

// This program detects input features influencing key points (the conditional branching points and the call to a function via function pointers) in a program
/*
Consider the following example program.

// Example 2.1
int main(){
   int id;
   int n;
   scanf(“%d, %d”, &id, &n);
   int s = 0;
   for (int i=0;i<n;i++){
      s += rand();
   }
   printf(“id=%d; sum=%d\n”, id, n); 
}

Based on def-use relations, we can figure out that only the second input integer determines the key points of this program 
(the branching decisions of the “for” loop) and hence its runtime. 
The output of the tool should indicate what the seminal input features are for the given program. For Example 2.1, the output can be as follows:

      Line 4: n

*/

// Pass ID variable  
char InputFeatureDetector::ID = 0;
/*
Method: A possible way to solve the problem is to use def-use relations to infer what part of the input is 
related with the key points in the program that determine the execution time of a program. 
For C programs, those points include branching points 
(including if-else, switch, loop condition checks, and so on), and function pointers
*/
// Run analysis on module
// This is almost identical to the part 1 code. TODO consolidate this and part 1 code
bool InputFeatureDetector::runOnModule(Module &M) 
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
                    filename = debugInfo -> getFilename().str();        // get the filename
                }

                if (BranchInst *BI = dyn_cast<BranchInst>(&I))          // if the instruction is a branch instruction
                    if ( BI -> isConditional() )                        // and a conditional branch
                        // printExecutedBranchInfo(Context, BI, M);
                        detectBranch(Context, BI, M);

                if (CallInst *CI = dyn_cast<CallInst>(&I))              // if the instruction is a call instruction
                    // printFunctionPtr(Context, CI, F, M);
                    detectCall(Context, CI, F, M);
            }
        }
    }

    writeToOutfile(llvm::sys::path::filename(filename).str());
    return true; // module was modified
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
/*
Hint, to deal with the more complicated cases like the second example, 
it may be necessary to make the compiler take advantage of the semantics of the I/O APIs 
by hardcoding the semantics of the relevant APIs inside your compiler-based analysis.
*/
// Write output to file
void InputFeatureDetector::writeToOutfile(std::string filename/*, std::vector<std::pair<std::string, std::string>> *branchDict*/) {

  // TODO: Implement output writing
  
}

// Detect input features influencing key points for branch instructions 
void InputFeatureDetector::detectBranch(LLVMContext& Context, BranchInst *BI, Module &M)
{
// Ensure that the branch instruction is conditional
    if (!BI->isConditional()) {
        return;
    }

    // Get the condition of the branch
    Value *condition = BI->getCondition();

    // Analyze the condition
    if (ICmpInst *cmp = dyn_cast<ICmpInst>(condition)) {
        // The condition is a comparison instruction

        // Get the operands of the comparison
        Value *leftOperand = cmp->getOperand(0);
        Value *rightOperand = cmp->getOperand(1);

        // Process the operands to find seminal input features
        if (isa<Constant>(leftOperand) || isa<Constant>(rightOperand)) {
            // Constants take highest precedence
            if (isa<Constant>(leftOperand)) {
                processOperand(leftOperand); 
            } else {
                processOperand(rightOperand);
            }
        } else if (isa<Argument>(leftOperand) || isa<Argument>(rightOperand)) {
            if (isa<Argument>(leftOperand)) {
                processOperand(leftOperand);
            } else {
                processOperand(rightOperand);
            }
        } else if(isa<CallInst>(leftOperand) || isa<CallInst>(rightOperand) || isa<LoadInst>(leftOperand) || isa<LoadInst>(rightOperand)) {
            if (isa<CallInst>(leftOperand) && isa<LoadInst>(rightOperand)) { // left operand is seminal input i = 0; foo() > i; i++
                processOperand(leftOperand);
            } else if (isa<CallInst>(rightOperand) && isa<LoadInst>(leftOperand)) { // right operand is seminal input i = 0; i < foo(); i++
                processOperand(rightOperand);
            } else if(isa<LoadInst>(leftOperand) && isa<LoadInst>(rightOperand)){ // both operands are variables i = 0; i < n; i++
                Value *seminalInput = determineLoopBound(Context, BI, M, leftOperand, rightOperand); // determine which operand is the loop counter(seminal input)
                processOperand(seminalInput);
            } else { // condition is not processed here
                return;
            }
        }
    } else if (isa<CallInst>(condition)) { // condition is a call instruction, FIXME: may remove this in case runOnModule() already can detect this. e.g. while(foo())
        detectCall(Context, dyn_cast<CallInst>(condition), *BI->getParent()->getParent(), M);
    } else { // condition evaluates to a value which is processed as either true or false 
       return;
    }

    // Additional checks and analyses might be needed for more complex conditions
    // For example, conditions involving function calls, arithmetic operations, etc.
    //TODO: Find more conditions that need to be analyzed
}

Value* determineLoopBound(LLVMContext &Context, BranchInst *BI, Module &M, Value *leftOperand, Value *rightOperand) {
    // First, determine if BI is part of a loop.
    LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    Loop *loop = LI.getLoopFor(BI->getParent());
    if (!loop) return nullptr; // Not part of a loop.

    // Check if either operand is a loop counter.
    bool leftIsCounter = isOperandLoopCounter(loop, leftOperand);
    bool rightIsCounter = isOperandLoopCounter(loop, rightOperand);

    // The operand that is not the counter is likely the loop bound.
    if (leftIsCounter && !rightIsCounter) {
        return rightOperand;
    } else if (!leftIsCounter && rightIsCounter) {
        return leftOperand;
    }

    // In case both or neither are counters, additional analysis may be required.
    return nullptr;
}

bool isOperandLoopCounter(Loop *loop, Value *operand) {
        // Ensure that the loop and operand are valid
    if (!loop || !operand) return false;

    // Iterate over all blocks in the loop
    for (BasicBlock *BB : loop->getBlocks()) {
        // Iterate over all instructions in the block
        for (Instruction &I : *BB) {
            // Check for instructions that could modify the operand (e.g., Add, Sub)
            if (auto *binOp = dyn_cast<BinaryOperator>(&I)) {
                // Check if the binary operation modifies the operand
                if ((binOp->getOpcode() == Instruction::Add || binOp->getOpcode() == Instruction::Sub) &&
                    (binOp->getOperand(0) == operand || binOp->getOperand(1) == operand)) {
                    // Check if the other operand is a constant (typical of loop counters)
                    Value *otherOperand = (binOp->getOperand(0) == operand) ? binOp->getOperand(1) : binOp->getOperand(0);
                    if (isa<ConstantInt>(otherOperand)) {
                        return true; // Operand is modified by a constant amount, typical of a loop counter
                    }
                }
            }
        }
    }

    return false; // Operand is not modified as a loop counter
}

void InputFeatureDetector::processOperand(Value *operand) {

    
    // Check if the operand is a constant int
    if (auto *constantInt = dyn_cast<ConstantInt>(operand)) {
        int value = constantInt->getSExtValue();
        
        // Print out the constant value
        errs() << "Input value: " << value << "\n";

    } else if (auto *inst = dyn_cast<Instruction>(operand)) {
        // Operand is an instruction

        // Trace back to the source 
        Value* source = traceToSource(inst);

        if (source) {
            // Found source instruction, print it out
            errs() << "Input variable: " << *source << "\n";
        }
    }
}

// Recursively trace instruction to source
Value* traceToSource(Instruction* inst) {
    if (inst && (isa<Argument>(inst) || isa<CallInst>(inst))) {
        return inst;
    }
    if (inst && inst->getNumOperands() > 0) {
        return traceToSource(dyn_cast<Instruction>(inst->getOperand(0)));
    }
    return nullptr;
}

// Detect input features influencing key points for call instructions
void InputFeatureDetector::detectCall(LLVMContext& Context, CallInst *CI, Function &F, Module &M)
{

}

// this registers the branch-pointer-tracer pass with the LLVM
static RegisterPass<InputFeatureDetector> X("input-pointer-tracer", "Part2: Input-Pointer-Tracer");