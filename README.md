# CSC412 Project Development
Alex Zettlemoyer, Willie Harris

PART 1:
- llvm must be installed and built
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../llvm-project/llvm
cmake --build ./

- to compile the target source code file with clang:
clang -S -emit-llvm inputfile.c -o inputfile.ll

- to build the branch-pointer-tracer pass:
clang++ -shared -o bin/BranchTracer.so Part1/BranchTracer.cpp `llvm-config --cxxflags --ldflags --libs`

- to run the LLVM on inputfile.ll with the branch-pointer-tracer:
opt -load bin/BranchTracer.so -branch-pointer-tracer < inputfile.ll > outputfile.ll
