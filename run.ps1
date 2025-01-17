# Check if the bin directory exists
if (-not (Test-Path -Path "bin")) {
    Write-Host "Bin directory does not exist. Please build the project first."
    exit 1
}

$binDirectory = "bin"
$files = Get-ChildItem -Path $binDirectory

# List all binaries in the bin directory
Write-Host "Running all binaries in the 'bin' directory..."
foreach ($file in $files) {
    if ($file.Name -match '\.exe$') {
        Write-Host "Running $($file.Name)..."
        & $file.FullName
        Write-Host ""
    } else {
        Write-Host "Skipping $($file.Name) (Not executable)."
    }
}

Write-Host "Run complete."
