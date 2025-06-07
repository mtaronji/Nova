
# Path to glslangValidator (adjust if needed)
$SHADER_COMPILER_PATH = "${env:VULKAN_SDK}\Bin\glslangValidator.exe"

# Get the directory where this script is located
$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Definition

# Get all .vert and .frag files in the same directory as this script
$VERT_FILES = Get-ChildItem -Path $SCRIPT_DIR -Filter *.vert
$FRAG_FILES = Get-ChildItem -Path $SCRIPT_DIR -Filter *.frag

# Compile each vertex shader
foreach ($VERT_FILE in $VERT_FILES) {
    $OUTPUT_FILE = "$($VERT_FILE.FullName).spv"
    Write-Host "Compiling vertex shader: $($VERT_FILE.FullName) -> $OUTPUT_FILE"
    & $SHADER_COMPILER_PATH -V $VERT_FILE.FullName -o $OUTPUT_FILE
}

# Compile each fragment shader
foreach ($FRAG_FILE in $FRAG_FILES) {
    $OUTPUT_FILE = "$($FRAG_FILE.FullName).spv"
    Write-Host "Compiling fragment shader: $($FRAG_FILE.FullName) -> $OUTPUT_FILE"
    & $SHADER_COMPILER_PATH -V $FRAG_FILE.FullName -o $OUTPUT_FILE
}

Write-Host "Shader compilation complete."