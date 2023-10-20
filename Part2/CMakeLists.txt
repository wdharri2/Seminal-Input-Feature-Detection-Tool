cmake_minimum_required(VERSION 3.12)

# Part 1 pass
#project(BranchTracer) 

#find_package(LLVM REQUIRED CONFIG)

#add_definitions(${LLVM_DEFINITIONS})
#include_directories(${LLVM_INCLUDE_DIRS})

#add_library(BranchTracer MODULE BranchTracer.cpp) 
#target_link_libraries(BranchTracer PRIVATE ${LLVM_LIBS})


# Part 2 pass 
project(InputFeatureDetector)

find_package(LLVM REQUIRED CONFIG)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

add_library(InputFeatureDetector MODULE InputFeatureDetector.cpp)
target_link_libraries(InputFeatureDetector PRIVATE ${LLVM_LIBS})