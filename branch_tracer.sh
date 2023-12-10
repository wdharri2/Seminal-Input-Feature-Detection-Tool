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
echo -e "**** Generating LLVM IR for ${C_FILE_PATH} ..."
clang -O0 -g -S -emit-llvm "../$C_FILE_PATH" -o "../bin/${file}.ll"

# Step 2: Compile BranchTracer.cpp to a shared object
echo -e "**** Compiling BranchTracer.cpp ..."
clang++ -shared -o ../bin/BranchTracer.so ../Part1/BranchTracer.cpp $(llvm-config --cxxflags --ldflags --libs) -fPIC

# Step 3: Transform LLVM IR using BranchTracer.so
echo -e "\n**** Transforming ${C_FILE_PATH} to /bin/${file}.ll ..."
opt -enable-new-pm=0 -load ../bin/BranchTracer.so -branch-pointer-tracer < "../bin/${file}.ll" > "../bin/transformed_${file}.ll"

cd ../

# Step 4: Make the transformed file executable
chmod +x "bin/transformed_${file}.ll"

# Step 5: Execute the transformed file
echo -e "\n**** Running the transformed file: ./bin/transformed_${file}.ll"
"./bin/transformed_${file}.ll"

# Step 6: Compile the original C file
echo -e "\n\b**** compiling original C file to /bin/${file}"
gcc "$C_FILE_PATH" -o "bin/${file}"

# Step 7: Run Valgrind callgrind tool
echo -e "**** Running /bin/${file} with callgrind\n"
valgrind --tool=callgrind --callgrind-out-file=callgrind_output.txt ./bin/${file} {@:2} 2> >(grep -E '^==.*callgrind.*==' | tee callgrind_output.txt >&2)
collected_number=$(grep -oE 'totals: [0-9]+' callgrind_output.txt | awk '{print $NF}')
echo -e "\n**** Number of executed instructions (via callgrind_output.txt): $collected_number"
