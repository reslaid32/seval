#!/bin/bash

# Check if the bin directory exists
if [ ! -d "bin" ]; then
    echo "Bin directory does not exist. Please build the project first."
    exit 1
fi

# Run the C++14 binaries
echo "Running C++14 binaries..."
echo "Running bin/b14..."
./bin/b14
echo "Running bin/t14..."
./bin/t14

# Run the C++17 binaries
echo "Running C++17 binaries..."
echo "Running bin/b17..."
./bin/b17
echo "Running bin/t17..."
./bin/t17

echo "Run complete."
