cmake_minimum_required(VERSION 4.0.2)  # CMake version check
project(nova)               # Create project "nova"
set(CMAKE_CXX_STANDARD 23)         # Enable c++23 standard

if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config generators like Visual Studio
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/x64/$<CONFIG>")
 
else()
    # Single-config generators like Make/Ninja on Linux
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/$<CONFIG>")
endif()

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

#generate information based on build type so we can find our executable paths or other important information that changes when you change platforms

configure_file(
    ${CMAKE_SOURCE_DIR}/generated/config.hpp.in
    ${CMAKE_SOURCE_DIR}/generated/config.hpp
)

# Set compiler flags for each build type
if(MSVC)
    # MSVC debug flags: /Zi = debug info, /Od = disable optimizations for debugging
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od")
    # MSVC release flags: optimize, no debug info
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    # GCC/Clang debug flags: -g for debug info, -O0 disables optimization
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    # GCC/Clang release flags: optimize, no debug info
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Freetype REQUIRED)

add_subdirectory(shaders)
add_subdirectory(common)
add_subdirectory(kindle)
add_subdirectory(nova)


#BUILDING NOVA
# Recursively find source and header files

file(GLOB_RECURSE SHADERS CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "shaders/*.vert" "shaders/*.frag")
file(GLOB_RECURSE PIPELINES CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "pipelines/*.json")
file(GLOB_RECURSE RENDERPASSES CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "renderpasses/*.json")
file(GLOB_RECURSE FONTLOADERHEADERS CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "fontloader/include/*.h" "fontloader/include/*.hpp")
file(GLOB_RECURSE FONTLOADERSRC CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "fontloader/src/*.cpp" "fontloader/src/*.c")
file(GLOB_RECURSE DESCRIPTORS CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "descriptors/*.json")
file(GLOB_RECURSE SCRIPTS CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "scripts/*.ps1" "scripts/*.sh")
file(GLOB_RECURSE CONFIG CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "config/*.json")
file(GLOB_RECURSE CONFIG_GENERATION CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "generated/*.hpp" "generated/*.in")



#want the main.cpp in the root
set(ENTRY "${CMAKE_SOURCE_DIR}/main.cpp")


source_group(TREE "${CMAKE_SOURCE_DIR}/fontloader/include" PREFIX "fontloader/include" FILES ${FONTLOADERHEADERS})
source_group(TREE "${CMAKE_SOURCE_DIR}/fontloader/src" PREFIX "fontloader/source" FILES ${FONTLOADERSRC})
source_group(TREE "${CMAKE_SOURCE_DIR}/shaders" PREFIX "shaders" FILES ${SHADERS})
source_group(TREE "${CMAKE_SOURCE_DIR}/pipelines" PREFIX "pipelines" FILES ${PIPELINES})
source_group(TREE "${CMAKE_SOURCE_DIR}/renderpasses" PREFIX "renderpasses" FILES ${RENDERPASSES})
source_group(TREE "${CMAKE_SOURCE_DIR}/descriptors" PREFIX "descriptors" FILES ${DESCRIPTORS})
source_group(TREE "${CMAKE_SOURCE_DIR}/scripts" PREFIX "scripts" FILES ${SCRIPTS})
source_group(TREE "${CMAKE_SOURCE_DIR}/config" PREFIX "config" FILES ${CONFIG})
source_group(TREE "${CMAKE_SOURCE_DIR}/generated" PREFIX "generated" FILES ${CONFIG_GENERATION})
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${ENTRY})

add_executable(main)


# Add both sources AND headers (for IDE indexing)
target_sources(main PRIVATE 
    main.cpp  # explicitly add the root main.cpp file

    ${FONTLOADERSRC}
    ${FONTLOADERHEADERS}  # add so ide can create directory strucure 
    ${SHADERS}            # Optional (shaders are also assets)
    ${PIPELINES}          # Adds pipeline JSON files to IDE
    ${RENDERPASSES}       # Adds renderpass JSON files to IDE
    ${DESCRIPTORS}     # Adds descriptor set JSON files to IDE
    ${SCRIPTS}            # adds scripts folder to the ide
    ${CONFIG_GENERATION}  #generated data for cross platform paths, etc
    ${CONFIG}             #for config schema files for pipeline render pass, etc to add to IDE
)

target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include
                                        ${CMAKE_SOURCE_DIR}/fontloader/include
                                        ${FREETYPE_INCLUDE_DIRS}
                                        )

# Link libraries
target_link_libraries(main
    PRIVATE
    nova
    common
    ${FREETYPE_LIBRARIES}
)


