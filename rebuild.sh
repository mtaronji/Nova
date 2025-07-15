# ~/cmake_rebuild.sh

#!/bin/bash
set -e

echo "Rebuilding project with CMake..."
rm -r ./build
cmake -S . -B ./build