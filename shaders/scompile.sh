#!/bin/bash

# Define the shader compiler (assuming you're using Google's shader compiler)
SHADER_COMPILER_PATH="/usr/bin/glslangValidator"  

# Get the directory where the script is located
SCRIPT_DIR="$(dirname "$(realpath "$0")")"

# Find all .vert and .frag files in the current directory
VERT_FILES=$(find "$SCRIPT_DIR" -maxdepth 1 -name "*.vert")
FRAG_FILES=$(find "$SCRIPT_DIR" -maxdepth 1 -name "*.frag")

# Compile each vertex shader
for VERT_FILE in $VERT_FILES; do
    # Output file will have the same name but with .spv extension
    OUTPUT_FILE="${VERT_FILE}.spv"
    echo "Compiling vertex shader: $VERT_FILE -> $OUTPUT_FILE"
    "$SHADER_COMPILER_PATH" -V "$VERT_FILE" -o "$OUTPUT_FILE"
done

# Compile each fragment shader
for FRAG_FILE in $FRAG_FILES; do
    OUTPUT_FILE="${FRAG_FILE%}.spv"
    echo "Compiling fragment shader: $FRAG_FILE -> $OUTPUT_FILE"
    "$SHADER_COMPILER_PATH" -V "$FRAG_FILE" -o "$OUTPUT_FILE"
done

echo "Shader compilation complete."