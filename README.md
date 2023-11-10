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

`clang -g -S -emit-llvm ../tests/example.c -o example.ll`

- to build the branch-pointer-tracer pass:

clang++ -shared -o ../bin/BranchTracer.so ../Part1/BranchTracer.cpp `llvm-config --cxxflags --ldflags --libs` -fPIC

- to run the LLVM on inputfile.ll with the branch-pointer-tracer:

`opt -enable-new-pm=0 -load ../bin/BranchTracer.so -branch-pointer-tracer < example.ll > transformed_example.ll`

- you may have to grant yourself permission to run the transformed output file

`chmod +x transformed_example.ll`
`./transformed_example.ll`

- this will output a Branch Dictionary txt file to the output folder with the path

`output/example.c_BranchDictionary.txt`

