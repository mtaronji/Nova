
# Path to glslangValidator (adjust if needed)
$glslangValidator = "${env:VULKAN_SDK}\Bin\glslangValidator.exe"

# Verify the validator exists
if (-not (Test-Path $glslangValidator)) {
    Write-Error "glslangValidator not found. Make sure Vulkan SDK is installed and VULKAN_SDK environment variable is set."
    exit 1
}

# Define source and destination directories
$shaderDir = "shaders"
$outputDir = "build/compiled_shaders"

# Create output directory if it doesn't exist
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir | Out-Null
}

# Compile all .vert and .frag shaders
Get-ChildItem -Path $shaderDir -Recurse -Include *.vert, *.frag | ForEach-Object {
    $inputPath = $_.FullName
    $fileName = $_.BaseName + $_.Extension + ".spv"
    $outputPath = Join-Path $outputDir $fileName

    Write-Output "Compiling $inputPath to $outputPath"

    & $glslangValidator -V $inputPath -o $outputPath

    if ($LASTEXITCODE -ne 0) {
        Write-Error "Failed to compile $inputPath"
        exit $LASTEXITCODE
    }
}