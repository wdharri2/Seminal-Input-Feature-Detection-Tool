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
                if ( filename.empty() && I.getDebugLoc())
                {
                    const DebugLoc &debugInfo = I.getDebugLoc();
                    filename = debugInfo -> getFilename().str();        // get the filename
                }
                if (BranchInst *BI = dyn_cast<BranchInst>(&I)){          // if the instruction is a branch instruction
                    if ( BI -> isConditional() ){                        // and a conditional branch
                        // printExecutedBranchInfo(Context, BI, M);
                        // errs() << "Branch instruction " + std::to_string(BI->getDebugLoc().getLine()) << "\n";
                        detectBranch(BI);
                    }
                }
                if (CallInst *CI = dyn_cast<CallInst>(&I))              // if the instruction is a call instruction
                    // printFunctionPtr(Context, CI, F, M);
                    detectCall(Context, CI, F, M);
            }
        }
    }

    // writeToOutfile(llvm::sys::path::filename(filename).str());
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

// Detect input features influencing key points for branch instructions 
void InputFeatureDetector::detectBranch(BranchInst *BI)
{
// Ensure that the branch instruction is conditional
    if (!BI->isConditional()){
        return;
    }

     // Get the condition of the branch
    Value *condition = BI->getCondition();

    // if branch instruction is an if-else statement runtime is dependent on BasicBlock successors
    // check if the branch instruction is an if-else statement
    std::string line = getLineFromFile(BI->getDebugLoc().getLine(), BI->getDebugLoc()->getFilename().str());
    if(startsWithControlStructure(line)){
        errs() << "Line " << BI->getDebugLoc().getLine() << ": if-else branch length of `";
        //remove leading and trailing whitespace from line
        std::string lineNoWhitespace = trim(line);

        errs() << lineNoWhitespace << "`\n";
        return;
    }

    // Analyze the condition
    if (ICmpInst *cmp = dyn_cast<ICmpInst>(condition)) {
        // The condition is a comparison instruction

        // Get the operands of the comparison
        Value *leftOperand = cmp->getOperand(0);
        Value *rightOperand = cmp->getOperand(1);

        // Check if the comparison is an equality comparison becuase if it is then the condition is processed differently
        if(cmp->getPredicate() == CmpInst::Predicate::ICMP_EQ) {
            // Get the operands of the comparison
            errs() << "Line " << cmp->getDebugLoc().getLine() << ": ";
            leftOperand->printAsOperand(errs(), false, nullptr);
            errs() << " == ";
            rightOperand->printAsOperand(errs(), false, nullptr);
            errs() << "\n";
            return;
        }

        // Process the operands to find seminal input features
        if (isa<Constant>(leftOperand) || isa<Constant>(rightOperand)) {
            // Constants take highest precedence
            if (isa<Constant>(leftOperand)) {
                errs() << "Line " << cmp->getDebugLoc().getLine() << ": ";
                leftOperand->printAsOperand(errs(), false, nullptr);
                errs() << "\n";
            } else {
                errs() << "Line " << cmp->getDebugLoc().getLine() << ": ";
                rightOperand->printAsOperand(errs(), false, nullptr);
                errs() << "\n";
            }
        } else if (isa<Argument>(leftOperand) || isa<Argument>(rightOperand)) {
            if (isa<Argument>(leftOperand)) {
                errs() << "Line " << cmp->getDebugLoc().getLine() << ": ";
                leftOperand->printAsOperand(errs(), false, nullptr);
                errs() << "\n";
            } else {
                errs() << "Line " << cmp->getDebugLoc().getLine() << ": ";
                rightOperand->printAsOperand(errs(), false, nullptr);
                errs() << "\n";
            }
        } else if(isa<CallInst>(leftOperand) || isa<CallInst>(rightOperand) || isa<LoadInst>(leftOperand) || isa<LoadInst>(rightOperand)) {
            if (isa<CallInst>(leftOperand) && isa<LoadInst>(rightOperand)) { // left operand is seminal input i = 0; foo() > i; i++
                errs() << "Line " << cmp->getDebugLoc().getLine() << ": ";
                leftOperand->printAsOperand(errs(), false, nullptr);
                errs() << "\n";
            } else if (isa<CallInst>(rightOperand) && isa<LoadInst>(leftOperand)) { // right operand is seminal input i = 0; i < foo(); i++
                errs() << "Line " << cmp->getDebugLoc().getLine() << ": ";
                rightOperand->printAsOperand(errs(), false, nullptr);
                errs() << "\n";
            } else if(isa<LoadInst>(leftOperand) && isa<LoadInst>(rightOperand)){ // both operands are variables i = 0; i < n; i++
                std::string leftOperandLine = getLineFromFile(dyn_cast<Instruction>(leftOperand)->getDebugLoc().getLine(), dyn_cast<Instruction>(leftOperand)->getDebugLoc()->getFilename().str());                BasicBlock *BB = BI->getSuccessor(0);
                auto operands = parseCondition(leftOperandLine);
                errs() << "Line " << cmp->getDebugLoc().getLine() << ": ";
                errs() << operands[0] << "compared to " << operands[1] << "\n";
            } else { // condition is not processed here
                return;
            }
        }
    } else if (isa<CallInst>(condition)) { // condition is a call instruction, FIXME: may remove this in case runOnModule() already can detect this. e.g. while(foo())
        //detectCall(Context, dyn_cast<CallInst>(condition), *BI->getParent(), M);
    } else { // condition evaluates to a value which is processed as either true or false 
       return;
    }

    // Additional checks and analyses might be needed for more complex conditions
    // For example, conditions involving function calls, arithmetic operations, etc.
}

// Recursively trace instruction to source
Value* InputFeatureDetector::traceToSource(Instruction* inst) {
    if (inst && (isa<Argument>(inst) || isa<CallInst>(inst))) {
        return inst;
    }
    if (inst && inst->getNumOperands() > 0) {
        return traceToSource(dyn_cast<Instruction>(inst->getOperand(0)));
    }
    return nullptr;
}

// Detect input features influencing key points for call instructions
void InputFeatureDetector::detectCall(LLVMContext& Context, CallInst *CI, Function &F, Module &M) {

    Function* calledFunc = CI->getCalledFunction(); 

    if (!calledFunc) {
        // Indirect call, not handled for now
        return;
    }

    // Check if it's a file I/O call
    if (calledFunc->getName() == "fopen" || calledFunc->getName() == "open") {

        // fopen/open defines the file to be processed

        Value* filenameParam = CI->getArgOperand(0); 

        std::string line = getLineFromFile(CI->getDebugLoc().getLine(), CI->getDebugLoc()->getFilename().str());
        std::string variableName = extractVariableName(line);

        // Filename is a global constant string
        errs() << "Line " << CI->getDebugLoc().getLine() << ": " << "Length of file " << variableName << "\n";


    } else if(calledFunc->getName() == "gets" || calledFunc->getName() == "getc") { // Line 3: size of stdin
        errs() << "Line " << CI->getDebugLoc().getLine() << ": Size of stdin\n";
    }
}

void InputFeatureDetector::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequired<LoopInfoWrapperPass>();
}

std::string InputFeatureDetector::getLineFromFile(int lineNumber, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }

    std::string line;
    for (int i = 0; i < lineNumber; ++i) {
        if (!getline(file, line)) {
            return "";
        }
    }

    return line;
}

bool InputFeatureDetector::startsWithControlStructure(const std::string& line) {
    // Lambda to check if a character is not a whitespace or closing bracket
    auto not_space_or_bracket = [](char ch) {
        return !std::isspace(static_cast<unsigned char>(ch)) && ch != '}';
    };

    // Find the first non-whitespace and non-closing bracket character
    auto first_char = std::find_if(line.begin(), line.end(), not_space_or_bracket);

    // Check if string starts with specific control structures
    if (std::distance(line.begin(), first_char) <= line.size()) {
        if (line.compare(std::distance(line.begin(), first_char), 2, "if") == 0) {
            return true;
        }
        if (line.compare(std::distance(line.begin(), first_char), 7, "else if") == 0) {
            return true;
        }
        if (line.compare(std::distance(line.begin(), first_char), 4, "else") == 0) {
            return true;
        }
    }
    return false;
}


std::string InputFeatureDetector::trim(const std::string& str) {
    auto start = std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    auto end = std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base();

    return (start < end) ? std::string(start, end) : "";
}

std::string InputFeatureDetector::extractVariableName(const std::string& line) {
    std::regex pattern(R"(\bFILE\s*\*\s*(\w+)\s*=|^\s*\*\s*(\w+)\s*=)");
    std::smatch matches;

    if (std::regex_search(line, matches, pattern)) {
        for (size_t i = 1; i < matches.size(); ++i) {
            if (!matches[i].str().empty()) {
                return matches[i];
            }
        }
    }
    return "";
}

std::vector<std::string> InputFeatureDetector::parseCondition(const std::string& str) {
    std::regex pattern(R"(\(([^,]+)\s*(<|<=|==|!=|>=|>)\s*([^,]+)\))");
    std::smatch matches;
    std::vector<std::string> operands;

    if (std::regex_search(str, matches, pattern) && matches.size() >= 4) {
        operands.push_back(matches[1].str()); // Left operand
        operands.push_back(matches[3].str()); // Right operand
    }

    return operands;
}

// this registers the branch-pointer-tracer pass with the LLVM
static RegisterPass<InputFeatureDetector> X("input-pointer-tracer", "Part2: Input-Pointer-Tracer");