# Set absolute paths
$baseDir = (Get-Item $PSScriptRoot).Parent.FullName
$packageDir = Join-Path $baseDir "package\release"
$buildDir = Join-Path $baseDir "build\release"

Write-Host "Base directory: $baseDir"
Write-Host "Build directory: $buildDir"
Write-Host "Package directory: $packageDir"

# Clean and create package directory
if (Test-Path $packageDir) {
    Write-Host "Removing existing package directory..."
    Remove-Item -Recurse -Force $packageDir -ErrorAction Stop
}
Write-Host "Creating package directory..."
New-Item -ItemType Directory -Path $packageDir -Force -ErrorAction Stop

# Check if build directory exists
if (-not (Test-Path $buildDir)) {
    Write-Host "Error: Build directory does not exist. Please build the project first." -ForegroundColor Red
    exit 1
}

# List files in build directory
Write-Host "Files in build directory:"
Get-ChildItem -Path $buildDir | Format-Table -Property Name, Length

# Copy executable file
$exePath = Join-Path $buildDir "ProductChecker.exe"
if (Test-Path $exePath) {
    Write-Host "Copying executable file..."
    Copy-Item $exePath $packageDir -ErrorAction Stop
    Write-Host "Executable file copied successfully."
} else {
    Write-Host "Error: ProductChecker.exe not found in build directory." -ForegroundColor Red
    exit 1
}

# Copy all DLL files
Write-Host "Copying DLL files..."
$dllFiles = Get-ChildItem -Path $buildDir -Filter "*.dll"
Write-Host "Found $($dllFiles.Count) DLL files."
foreach ($dll in $dllFiles) {
    Write-Host "Copying $($dll.Name)..."
    Copy-Item $dll.FullName $packageDir -ErrorAction Stop
}
Write-Host "DLL files copied successfully."

# Copy all plugin directories
Write-Host "Copying plugin directories..."
$pluginDirs = @("generic", "iconengines", "imageformats", "networkinformation", "platforms", "styles", "tls", "translations")
foreach ($dir in $pluginDirs) {
    $sourceDir = Join-Path $buildDir $dir
    $targetDir = Join-Path $packageDir $dir
    if (Test-Path $sourceDir) {
        Write-Host "Copying $dir directory..."
        New-Item -ItemType Directory -Path $targetDir -Force -ErrorAction Stop
        $files = Get-ChildItem -Path $sourceDir -Recurse
        Write-Host "Found $($files.Count) files in $dir directory."
        foreach ($file in $files) {
            Write-Host "Copying $($file.Name)..."
            Copy-Item $file.FullName $targetDir -ErrorAction Stop
        }
        Write-Host "$dir directory copied successfully."
    } else {
        Write-Host "$dir directory not found."
    }
}

# Create start script
Write-Host "Creating start script..."
$startScript = "@echo off
ProductChecker.exe"
$startScriptPath = Join-Path $packageDir "start.bat"
$startScript | Out-File $startScriptPath -Encoding ASCII -ErrorAction Stop
Write-Host "Start script created successfully."

# Verify the package
Write-Host "Verifying package..."
$packageFiles = Get-ChildItem -Path $packageDir -Recurse
Write-Host "Package contains $($packageFiles.Count) files."
Write-Host "Files in package directory:"
Get-ChildItem -Path $packageDir | Format-Table -Property Name, Length

Write-Host "Packaging completed!" -ForegroundColor Green
Write-Host "Package files located at: $packageDir" -ForegroundColor Green
