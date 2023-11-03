# CSC412 Project Development
Alex Zettlemoyer, Willie Harris

PART 1:
- LLVM must be installed and built.
    To do so, from the project directory run

    - `mkdir build`
    - `cd build`
    - `cmake -DCMAKE_BUILD_TYPE=Debug ../llvm`
    - `cmake --build ./`

    ** all compilation / run instructions are from the build directory

- to compile the target source code file with clang:

`clang -g -S -emit-llvm ../tests/automaton.c -o automaton.ll`

- to build the branch-pointer-tracer pass:

clang++ -shared -o ../bin/BranchTracer.so ../Part1/BranchTracer.cpp `llvm-config --cxxflags --ldflags --libs` -fPIC

- to run the LLVM on inputfile.ll with the branch-pointer-tracer:

`opt -enable-new-pm=0 -load ../bin/BranchTracer.so -branch-pointer-tracer < automaton.ll > output.ll`

clang -o output_binary output.ll

- this will output a Branch Trace txt file to the output folder with the path

`output/automaton.c_BPT.txt`

