# CSC412 Project Development
Alex Zettlemoyer, Willie Harris

# Introduction

Understanding and predicting program behaviors is central to code optimization. Traditional compilers rely heavily on code-based estimations of these behaviors. While the source code is a crucial factor, it is not the only determinant of program behaviors; program inputs also play a significant role. A program may exhibit different behaviors depending on its inputs.

For specific types of program behaviors, such as running time, only certain features of the input are significant. For instance, in a matrix multiplication program, the exact values of the matrix elements do not impact the running time, but the shapes and dimensions of the matrices do. We refer to these critical input features that determine a program's behavior as “seminal input features.”

In this project, we developed a compiler-based tool to automatically recognize the “seminal input features” affecting a program's running time. The project consists of three main components:

1. **Key Point Recognition**: Identifying the critical points in a program that determine its running time.
2. **Input Feature Identification**: Determining the input features that influence the program's behavior at these key points.
3. **Tool Integration**: Combining the above components into a cohesive tool.

This tool aims to enhance the ability to understand and predict program behaviors, thereby facilitating more effective code optimization.

## Part #1: Profiling Dynamic Program Behavior

### Background

The execution time of a compiled C program is primarily influenced by two factors:

1. The decisions made at each branching point (conditional statements, loops, etc.).
2. The values of function pointers during the program's execution.

To accurately analyze a program's runtime performance, it is crucial to collect and examine these dynamic behaviors that occur during its execution.

### Objective

The primary objective of this project was to develop a profiling tool that generates a branch-pointer trace after a program's execution. This trace records the branching decisions and the values of function pointers encountered during the execution, formatted as follows:

```
*func_0x32576867 # the value of a function pointer when the function it points to is invoked
br_2
br_2
br_2
br_3
```

In the trace, "br_x" represents the ID of a branch in the program. The meaning of each branch ID is defined in a separate dictionary file produced by the tool, like this:

```
br_2: fileX, 5, 6
br_3: fileX, 5, 8
```

Here, "fileX" is the name of the source code file containing the branch, "5" is the line number of the branching statement, and "6" is the target line number for the branch taken.

### Additional Objective

The secondary objective was to create a binary profiling tool that reports the total number of executed instructions for a program after its execution. This was achieved using Valgrind.

### Hypothesis

The hypothesis was that for any two runs of a given program with different inputs, if their branch-pointer traces are identical, the numbers of executed instructions must also be identical (assuming that the ignored libraries do not behave differently). After developing the two profiling tools, this hypothesis was tested by running the program with different inputs that led to identical branch-pointer traces.

## Part #2: Identifying Seminal Input Features

### Objectives
The aim of this project was to develop a static analysis tool based on LLVM that can automatically determine which parts of the input to a C program influence the behaviors at its key points (as defined in Part #1: conditional branching points and calls to functions via function pointers). 

Consider the following example program:

```c
// Example 2.1
int main() {
    int id;
    int n;
    scanf("%d, %d", &id, &n);
    int s = 0;
    for (int i=0; i<n; i++) {
        s += rand();
    }
    printf("id=%d; sum=%d\n", id, n);
}
```

Based on def-use relations, it can be determined that only the second input integer affects the key points of this program (the branching decisions of the "for" loop) and thus its runtime. Sometimes, deeper semantics related to I/O and other APIs need to be considered. For instance, in the following program:

```c
// Example 2.2
int main() {
    char *str1[1000];
    File *fp = fopen("file.txt", "r");
    char c;
    int len = 0;
    while (1) {
        c = getc(fp);
        if (c == EOF) break;
        str1[len++] = c;
        if (len >= 1000) break;
    }
    printf("%s\n", str1);
}
```

The running time of the program is determined by the length of the input string. However, a naive def-use relation analysis might incorrectly conclude that the value of every character in the input string affects the program's runtime.

### Output
The tool's output should identify the seminal input features for the given program. For Example 2.1, the output might be:

```
Line 4: n
```

For Example 2.2, the output might be:

```
Line 3: size of file fp
```

The actual output format may vary; the above are merely examples.

### Method
One approach to solving this problem is to use def-use relations to infer which parts of the input relate to the key points in the program that determine its execution time. For C programs, these points include branching points (such as if-else, switch, loop condition checks) and function pointers, as described in Part #1. Despite the dependency on Part #1, students working on this part should not wait for the completion of Part #1. They can manually prepare records of the key points in the testing programs. If some programs are too complex to manually identify and record all key points, students should focus on those key points that critically determine the running time of the programs. To handle more complex cases like the second example, it may be necessary to hardcode the semantics of relevant I/O APIs into your compiler-based analysis. You are expected to add passes in LLVM to complete this project.

### Scope
This project focuses on C programs that may use a set of I/O APIs, including getc, fopen, scanf, fclose, fread, and fwrite. Inter-module analysis is out of scope, so each program will contain only one source file and some header files.

# Installation

To run the profiling tool, LLVM must be installed and built.

* To install LLVM clone from https://github.com/llvm-mirror/llvm or initialize submodule
* The cloned LLVM should be in our project directory, adjacent to the README.md file, titled "llvm"

* To build the cloned LLVM, from the project directory run

    - `mkdir build`
    - `cd build`
    - `cmake -DCMAKE_BUILD_TYPE=Debug ../llvm`
    - `cmake --build ./`

* This program requires the installation of clang, llvm, gcc, and valgrind

    - `sudo apt install clang`
    - `sudo apt install llvm`
    - `sudo apt install gcc`
    - `sudo apt install valgrind`

***** the LLVM build must be in a directory titled `build` in the project directory

_______
TESTING FILES:

* 2 small C programs (any of the files in /test) **TODO**

* real-world substitute to SPEC CPU: driver.c
    - driver.c is part of a C program implemented in CSC230
    - the program acts as a custom command prompt interface
    - this interface manages a custom implemented C map, which the user can manipulate through the command prompt
    - the map only works for <integer, integer> key-value pairs
    - the source file has 527 lines of non-comment, non-blank source code lines
        - use `cat tests/driver.c | sed '/^\s*$/d' | wc -l` to see the number
    - usage:
        - `set key value`: store a new key-value pair in the map
        - `get key`:          get the value of a key
        - `remove key`:       remove a key-value pair from the map
        - `size`:             get the current size of the map
        - `quit`:             quit the command prompt


_______
PART 1:

Run the script branch_tracer.sh for the full profiling tool
    `usage: ./branch_tracer.sh <path_to_input_C_file>`

for example, to run the profiling tool on example.c in the test directory:
    `./branch_tracer.sh tests/example.c`

This will
1. generate the LLVM IR for the input C file
2. compile the BranchTracer.cpp LLVM custom LLVM transform pass
3. transform the generated LLVM IR using the branch tracer
- this will output the static branch dictionary of all branches in the program, and their start and target lines
4. run the transformed file
- this will output the value of function pointers when they are invoked
- and the executed branches (from the branch dictionary)
5. copmile the original C file using gcc
6. run valgrind callgrind
- this will output the number of executed instructions (from valgrind's binary profiling tool)

_______
PART 2:

Run the script inputFeatureDetector.sh for the analysis tool
    `usage: ./inputFeatureDetector.sh <path_to_input_C_file>`

for example, to run the profiling tool on example.c in the test directory:
    `./inputFeatureDetector.sh tests/example.c`

This will
1. generate the LLVM IR for the input C file
2. compile the InputFeatureDetector.cpp LLVM custom LLVM transform pass
3. transform the generated LLVM IR using the branch tracer
- this will statically analyze the input file for key points

_______
PART 3:
Run the script start.sh for the tool to run cohesively
    `usage: ./start.sh <path_to_input_C_file>`

for example, to run the profiling and analysis tool on example.c in the test directory:
    `./start.sh tests/example.c`

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.
