cmake_minimum_required(VERSION 3.12)
project(BranchTracer)

# Find LLVM package
find_package(LLVM REQUIRED CONFIG)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

# Define BranchTracer pass source files
add_library(BranchTracer MODULE BranchTracer.cpp)

# Link BranchTracer against LLVM libraries
target_link_libraries(BranchTracer PRIVATE ${LLVM_LIBS})