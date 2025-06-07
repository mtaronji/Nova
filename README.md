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
🔹 Option A: Use vcpkg (Recommended for Windows)

vcpkg manages C++ libraries and integrates well with CMake and Visual Studio.

Setup vcpkg:

    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    .\bootstrap-vcpkg.bat          # On Windows
OR:

    ./bootstrap-vcpkg.sh           # On Linux/macOS

Install dependencies:

    .\vcpkg install glfw3 glm freetype vulkan

Integrate with build system:

    .\vcpkg integrate install

Build the project with vcpkg toolchain:

Replace the path with your actual vcpkg path:

    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 17 2022"
    cmake --build build --config Debug

Open the generated solution:

build/Nova.sln

🔹 Option B: Manual Dependency Setup

Build each dependency manually and set their paths:

    git clone https://github.com/glfw/glfw.git
    cd glfw
    cmake -B build -DCMAKE_INSTALL_PREFIX=C:/libs/glfw
    cmake --build build --target install

Repeat for GLM, FreeType, and others.

Build Nova with paths specified:

    cmake -B build -S . -DCMAKE_PREFIX_PATH="C:/libs/glfw;C:/libs/glm;C:/libs/freetype;C:/VulkanSDK/1.3.283.0" -G "Visual Studio 17 2022"
    cmake --build build --config Debug

🧠 Important Notes

    Use forward slashes (/) or escaped backslashes (\\) in Windows paths.

    Change "Visual Studio 17 2022" to your version if needed.

    The build system will automatically:

        Compile all .vert and .frag shaders in the shaders/ directory using glslangValidator.

        Copy compiled .spv shader files, pipeline configs, and renderpass configs to your runtime directory.

🧪 Linux Instructions

Ensure you install these via your package manager or manually:

    glfw

    glm

    freetype

    Vulkan SDK

Build the project:

    git clone https://github.com/mtaronji/nova.git
    cd nova
    cmake -B build -S .
    cmake --build build --config Debug

If dependencies are in non-default locations:

    cmake -B build -S . -DCMAKE_PREFIX_PATH="/opt/glfw;/opt/glm;/opt/freetype"

🔧 Build System Overview

    ALL_BUILD: A Visual Studio meta-target that builds all components.

    nova: Your main executable project.

    compile_shaders: A custom CMake target that:

        Automatically finds and compiles all GLSL shaders (.vert, .frag) to SPIR-V.

        Places output into the appropriate runtime /shaders folder.

To build shaders manually:

    cmake --build build --target compile_shaders --config Debug

📁 Runtime Directory Layout

At runtime, the following will be copied to your binary directory (build/x64/Debug or similar):

    nova.exe
    /shaders/            → compiled .spv files
    /pipelines/          → JSON pipeline configurations
    /renderpasses/       → JSON renderpass configs
    /descriptorsets/     → descriptor set JSONs
    /config/             → app-specific config

🎯 Summary

    ✅ Use vcpkg on Windows for the easiest setup.

    🛠️ Manual dependency setup also supported.

    🔁 Shaders auto-compile using a CMake target.

    🐧 Fully compatible with Linux and Visual Studio 2022.


    Cross-platform build support, but shader compilation is left to users.
