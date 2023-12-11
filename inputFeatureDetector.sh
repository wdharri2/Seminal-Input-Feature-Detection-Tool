#!/bin/bash

# Check if the argument (path to C file) is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <path_to_C_file>"
    exit 1
fi

C_FILE_PATH="$1"
filename=$(basename "$C_FILE_PATH")     # Extracts "example.c"
file="${filename%.*}"
mkdir -p bin   # Creates bin folder if it doesn't exist
cd build

# Step 1: Generate LLVM IR from the C file
echo -e "Generating LLVM IR for ${C_FILE_PATH}"
clang -g -S -emit-llvm "../$C_FILE_PATH" -o "../bin/${file}.ll"

# Step 2: Compile InputFeatureDetector.cpp to a shared object
echo -e "Compiling InputFeatureDetector.cpp"
clang++ -shared -o ../bin/InputFeatureDetector.so ../Part2/InputFeatureDetector.cpp $(llvm-config --cxxflags --ldflags --libs) -fPIC

# Step 3: Transform LLVM IR using InputFeatureDetector.so
echo -e "\nTransforming ${C_FILE_PATH} to /bin/${file}.ll"
opt -enable-new-pm=0 -load ../bin/InputFeatureDetector.so -input-pointer-tracer < "../bin/${file}.ll" > "../bin/transformed_${file}.ll"