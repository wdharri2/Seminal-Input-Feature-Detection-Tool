# CSC412 Project Development
Alex Zettlemoyer, Willie Harris

PART 1:
- LLVM must be installed and built.
    To do so, from the project directory run

    - `mkdir build`
    - `cd build`
    - `cmake -DCMAKE_BUILD_TYPE=Debug ../llvm-project/llvm`
    - `cmake --build ./`

    ** all compilation / run instructions are from the build directory

- to compile the target source code file with clang:

`clang -g -S -emit-llvm ../tests/example.c -o example.ll`

- to build the branch-pointer-tracer pass:

`clang++ -shared -o ../bin/BranchTracer.so ../Part1/BranchTracer.cpp ``llvm-config --cxxflags --ldflags --libs\`` -fPIC`

- to run the LLVM on inputfile.ll with the branch-pointer-tracer:

`opt -enable-new-pm=0 -load ../bin/BranchTracer.so -branch-pointer-tracer < example.ll > output.ll`


- to dynamically run the pass on program example.c

chmod +x trace_branches.sh
../trace_branches.sh ../tests/example.c