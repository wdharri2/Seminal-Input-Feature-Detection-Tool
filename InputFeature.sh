# script to run the LLVM with Branch Tracer pass on a C program

# Path to your LLVM pass shared library
PASS_LIB="../bin/InputFeatureDetector.so"

# Run the LLVM JIT interpreter (lli) with branch tracer
lli -load "$PASS_LIB" "$@"