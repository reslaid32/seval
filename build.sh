#!/bin/bash

# Create bin directory if it doesn't exist
mkdir -p bin

# Compile with C++14
clang++ -std=c++14 benchmark.cpp -o bin/b14
clang++ -std=c++14 test.cpp -o bin/t14

# Compile with C++17
clang++ -std=c++17 benchmark.cpp -o bin/b17
clang++ -std=c++17 test.cpp -o bin/t17

echo "Build complete. Binaries are in the 'bin' directory."
