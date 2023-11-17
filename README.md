# CSC412 Project Development
Alex Zettlemoyer, Willie Harris

PART 1:

To run the profiling tool, LLVM must be installed and built.
    To do so, from the project directory run

    - `mkdir build`
    - `cd build`
    - `cmake -DCMAKE_BUILD_TYPE=Debug ../llvm`
    - `cmake --build ./`

the LLVM build must be in a directory titled `build` in the project directory

Run the script branch_tracer.sh for the full profiling tool
    `usage: ./branch_tracer.sh <path_to_input_C_file>`

for example, to run the profiling tool on example.c in the test directory:
    `./branch_tracer.sh tests/example.c`

This will
1. generate the LLVM IR for the input C file
2. compile the BranchTracer.cpp LLVM custom LLVM transform pass
3. transform the generated LLVM IR using the branch tracer
* this will output the static branch dictionary of all branches in the program, and their start and target lines
4. run the transformed file
* this will output the value of function pointers when they are invoked
* and the executed branches (from the branch dictionary)
5. copmile the original C file using gcc
6. run valgrind callgrind
* this will output the number of executed instructions (from valgrind's binary profiling tool)

