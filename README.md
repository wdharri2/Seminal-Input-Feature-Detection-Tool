# CSC412 Project Development
Alex Zettlemoyer, Willie Harris

PART 1:
- llvm must be installed and built

- to build the branch-pointer-tracer pass:
run
cmake -DCMAKE_PREVIX=/path/to/llvm

cmake -DCMAKE_PREFIX_PATH=/home/ubuntu/compiler/CSC412_Project_Dev/llvm-project -DLLVM_DIR=/home/ubuntu/compiler/CSC412_Project_Dev/llvm-project/llvm/lib

- to load the branch-pointer-tracer pass in the LLVM:
- to run the LLVM with the branch-pointer-tracer:
