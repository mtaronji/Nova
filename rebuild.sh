# ~/cmake_rebuild.sh

#!/bin/bash
set -e

echo "Rebuilding project with CMake..."
cmake -S . -B ./build/debug