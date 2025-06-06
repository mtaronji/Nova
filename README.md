# Building the Nova Project

This project has been tested on:

- ✅ **Visual Studio 2022 (Windows)**  
- ✅ **Linux (with GCC/Clang and CMake)**

> ⚠️ Other environments may work but are untested.

---

## 🚀 Getting Started (Windows - Visual Studio)

### 📥 Clone the Repository

```powershell
git clone https://github.com/mtaronji/nova.git
cd repo

📦 Option 1: Use vcpkg for Dependencies

    This is the recommended method on Windows if you want find_package(...) to work out-of-the-box.
    1. Install vcpkg

    If you don’t have vcpkg installed yet, here’s how to get it:
    
    # Clone vcpkg repo
    git clone https://github.com/microsoft/vcpkg.git
    
    # Go into the vcpkg folder
    cd vcpkg
    
    # Bootstrap vcpkg (build the executable)
    .\bootstrap-vcpkg.bat   # on Windows PowerShell
    # or
    ./bootstrap-vcpkg.sh    # on Linux / macOS
    
    2. Install packages
    
    To install a library, just run:
    
    .\vcpkg install glfw3
    .\vcpkg install glm
    .\vcpkg install freetype
    
        You can install multiple packages at once:
    
    .\vcpkg install glfw3 glm freetype
    
    3. Integrate vcpkg with your build system
    
    Run this once (only need to do it once per user/machine):
    
    .\vcpkg integrate install
    
        This lets Visual Studio and CMake automatically find the installed packages.
    
    4. Using vcpkg with CMake
    
    When running CMake, specify the vcpkg toolchain file:
    
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 17 2022"
    
    Replace C:/path/to/vcpkg with your actual path.
    
    This tells CMake to use libraries installed via vcpkg, so your find_package() calls work correctly.
    
    Assuming vcpkg is installed at C:/tools/vcpkg:
    
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 17 2022"
    
    Then build the solution:
    
    cmake --build build --config Debug
    
    Open the solution in Visual Studio:
    
    build/Nova.sln

⚙️ Option 2: Manual Dependencies (Without vcpkg)

    If you're not using vcpkg, CMake's find_package(...) will fail unless you manually build and install your dependencies:

    Clone and build libraries like GLFW, GLM, and FreeType using CMake:

    git clone https://github.com/glfw/glfw
    cmake -B build -DCMAKE_INSTALL_PREFIX=C:/libs/glfw
    cmake --build build --target install
    
    Repeat for GLM, FreeType, etc.
    
        Then add them to your CMAKE_PREFIX_PATH:
    
    cmake -B build -S . -DCMAKE_PREFIX_PATH="C:/libs/glfw;C:/libs/glm;C:/libs/freetype" -G "Visual Studio 17 2022"

🔍 Notes on Windows

    Use forward slashes (/) or escaped backslashes (\\) in all CMake paths.

    Change "Visual Studio 17 2022" if you're using a different version.

    Toolchain files (like vcpkg) make your CMakeLists.txt cleaner and portable.

    -G specifies the CMake generator, i.e., what kind of project files to generate (Visual Studio, Ninja, Makefiles, etc.).

🐧 Linux Instructions

    Make sure all dependencies (GLFW, GLM, Freetype, etc.) are installed via your package manager or built manually and installed using CMake.
    
    Then build:
    
    git clone https://github.com/user/repo.git
    cd repo
    cmake -B build -S .
    cmake --build build --config Debug

    If dependencies were installed manually to a custom location, specify it like so:

    cmake -B build -S . -DCMAKE_PREFIX_PATH="/opt/glfw;/opt/glm;/opt/freetype"

🔨 How the Build System Works

    CMakeLists.txt automatically copies shaders, pipeline JSONs, and renderpasses to the output directory.

    If you're using Visual Studio, it may default to running ALL_BUILD — this is a meta target that builds all other targets.

    You can configure a specific project as the startup target inside Visual Studio (Right-click → Set as Startup Project).

🔁 Cross-Platform Support

    This project supports both Visual Studio and Linux builds. However, shader compilation (e.g., via glslangValidator) is not enforced automatically — the app assumes compiled .spv files exist.

    If you'd like to compile shaders manually:

    On Linux, you can configure CMake or a shell script to do it.

    On Windows, you can create a custom pre-build PowerShell task in Visual Studio.
