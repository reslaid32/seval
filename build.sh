#!/bin/bash

# Create bin directory if it doesn't exist
mkdir -p bin

# Define array of C++ standards
cpp_standards=("c++98" "c++03" "c++11" "c++14" "c++17")

# Loop through each standard and compile benchmark.cpp and test.cpp
for standard in "${cpp_standards[@]}"; do
    echo "Compiling with $standard..."
    clang++ -std=$standard -Iinclude benchmark.cpp -o "bin/b$(echo $standard | tr -d '+')"
    clang++ -std=$standard -Iinclude test.cpp -o "bin/t$(echo $standard | tr -d '+')"
done

echo "Build complete. Binaries are in the 'bin' directory."
