# Define the C compiler to use
CC = clang

# Add debug flags
CFLAGS += -g -S -emit-llvm

# Output LLVM IR instead of binaries
EXECUTABLES = $(SOURCES:%.c=%_gen.ll)
EXECUTABLES_GEN = $(SOURCES_GEN:%.c=%_gen.ll)

# Rule to build LLVM IR for each source file
%_gen.ll: %.c
	$(CC) $(CFLAGS) $< -o $@

# Build the branch pointer tracer plugin
BRANCH_TRACER = ../bin/BranchTracer.so

$(BRANCH_TRACER): ../Part1/BranchTracer.cpp
	clang++ -shared -o $@ $< `llvm-config --cxxflags --ldflags --libs` -fPIC

.PHONY: clean
clean:
	rm -f $(EXECUTABLES) $(EXECUTABLES_GEN) $(BRANCH_TRACER)
