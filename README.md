# CSC412 Project Development
Alex Zettlemoyer, Willie Harris

PART 1:
- llvm must be installed and built
    to do so, from the project directory run
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug ../llvm-project/llvm
    cmake --build ./

- to compile the target source code file with clang:
clang -S -emit-llvm ../path/to/inputfile.c -o inputfile.ll

- to build the branch-pointer-tracer pass:
clang++ -shared -o ../bin/BranchTracer.so ../Part1/BranchTracer.cpp `llvm-config --cxxflags --ldflags --libs` -fPIC

- to run the LLVM on inputfile.ll with the branch-pointer-tracer:
opt -enable-new-pm=0 -load ../bin/BranchTracer.so -branch-pointer-tracer < example.ll > outputfile.ll
