🚀 Building the Nova Project

This project has been tested on:

    ✅ Visual Studio 2022 (Windows)

    ✅ Linux (GCC/Clang + CMake)

    ⚠️ Other platforms may work but are untested.

💻 Getting Started
1. Clone the Repository

git clone https://github.com/mtaronji/nova.git
cd nova

2. Install Dependencies
Option A: Use vcpkg (Recommended on Windows)

vcpkg manages dependencies and integrates automatically with CMake and Visual Studio.
Setup vcpkg

# Clone vcpkg
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg

# Bootstrap (build) vcpkg executable
.\bootstrap-vcpkg.bat          # Windows PowerShell
# or
./bootstrap-vcpkg.sh           # Linux / macOS

Install packages

.\vcpkg install glfw3 glm freetype
# You can install multiple packages simultaneously:
.\vcpkg install glfw3 glm freetype

Integrate with build system (run once)

.\vcpkg integrate install

Build your project with vcpkg toolchain

Replace the path below with your vcpkg location:

cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 17 2022"
cmake --build build --config Debug

Open the solution:

build\Nova.sln

Option B: Manual Dependency Setup (Without vcpkg)

If you don’t want to use vcpkg:

    Clone and build each dependency yourself:

git clone https://github.com/glfw/glfw.git
cd glfw
cmake -B build -DCMAKE_INSTALL_PREFIX=C:/libs/glfw
cmake --build build --target install

Repeat for GLM, FreeType, etc.

    When building Nova, specify the dependencies locations:

cmake -B build -S . -DCMAKE_PREFIX_PATH="C:/libs/glfw;C:/libs/glm;C:/libs/freetype" -G "Visual Studio 17 2022"
cmake --build build --config Debug

📝 Important Notes

    Use forward slashes (/) or escaped backslashes (\\) in CMake paths on Windows.

    Change "Visual Studio 17 2022" to your installed VS version if different.

    The -G argument in CMake specifies the generator (e.g., Visual Studio, Ninja).

    Toolchain files like vcpkg keep your CMakeLists.txt clean and cross-platform.

    The build system automatically copies shaders, pipeline JSON files, and renderpasses to the output folder.

🐧 Linux Instructions

Ensure all dependencies (GLFW, GLM, FreeType) are installed via your package manager or built manually.

Build the project:

git clone https://github.com/mtaronji/nova.git
cd nova
cmake -B build -S .
cmake --build build --config Debug

If dependencies are in custom locations, specify:

cmake -B build -S . -DCMAKE_PREFIX_PATH="/opt/glfw;/opt/glm;/opt/freetype"

🔧 Build System Overview

    ALL_BUILD is a meta target Visual Studio creates to build all projects in the solution.

    Set your desired startup project by right-clicking it → Set as Startup Project in Visual Studio.

    The app expects compiled shaders (.spv) to exist. Shader compilation is not forced.

    For shader compilation:

        On Linux, you can add shell scripts or CMake custom commands.

        On Windows, add a PowerShell pre-build task in Visual Studio to compile shaders if desired.

🎯 Summary

    Use vcpkg on Windows for easiest dependency management.

    You can manually install libraries if preferred.

    Tested on Visual Studio 2022 and Linux.

    Cross-platform build support, but shader compilation is left to users.
