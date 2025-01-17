# Create bin directory if it doesn't exist
New-Item -ItemType Directory -Force -Path "bin"

# Define array of C++ standards
$cpp_standards = @("c++98", "c++03", "c++11", "c++14", "c++17")

# Loop through each standard and compile benchmark.cpp and test.cpp
foreach ($standard in $cpp_standards) {
    Write-Host "Compiling with $standard..."
    $benchmarkOutput = "bin/b" + $standard -replace '\+', ''
    $testOutput = "bin/t" + $standard -replace '\+', ''
    
    # Compile benchmark.cpp and test.cpp
    clang++ "-std=$standard" -Iinclude benchmark.cpp -o $benchmarkOutput
    clang++ "-std=$standard" -Iinclude test.cpp -o $testOutput
}

Write-Host "Build complete. Binaries are in the 'bin' directory."
