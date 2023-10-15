#ifndef LLVM_TRANSFORMS_BRANCH_TRACER_H
#define LLVM_TRANSFORMS_BRANCH_TRACER_H

#include "llvm/BranchTracer.h"

namespace llvm 
{
    class BranchTracer : public FunctionPass 
    {
        public:
            static char ID;
            BranchTracer() : FunctionPass(ID) {}

            bool runOnFunction(Function &F) override;
    };
}

#endif