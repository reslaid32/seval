#!/bin/bash

# Check if the bin directory exists
if [ ! -d "bin" ]; then
    echo "Bin directory does not exist. Please build the project first."
    exit 1
fi

# List all binaries in the bin directory
echo "Running all binaries in the 'bin' directory..."
for binary in bin/*; do
    if [ -x "$binary" ]; then
        echo "Running $binary..."
        "$binary"
        echo ""
    else
        echo "Skipping $binary (not executable)."
    fi
done

echo "Run complete."
