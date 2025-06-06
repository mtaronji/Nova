cmake_minimum_required(VERSION 4.0.2)  # CMake version check
project(nova)               # Create project "magus"
set(CMAKE_CXX_STANDARD 23)         # Enable c++23 standard

if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config generators like Visual Studio
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/x64/$<CONFIG>")
else()
    # Single-config generators like Make/Ninja on Linux
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}")
endif()


# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

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

#COPY SHADERS TO WHERE THE EXECUTABLE IS
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})

#COPY ALL PIPELINE FILES EXCEPT THE SCHEMA FILE TO THE EXECUTABLE DIRECTORY
file(GLOB PIPELINE_FILES "${CMAKE_SOURCE_DIR}/pipelines/*")
foreach(file ${PIPELINE_FILES})
    get_filename_component(FILENAME "${file}" NAME)

    if(NOT FILENAME STREQUAL "pipeline.schema.json")
        file(COPY "${file}" DESTINATION "${CMAKE_BINARY_DIR}/pipelines")
    endif()
endforeach()

#SAME AS PIPELINE ABOVE
file(GLOB RENDERPASS_FILES "${CMAKE_SOURCE_DIR}/renderpasses/*")
foreach(file ${RENDERPASS_FILES})
    get_filename_component(FILENAME "${file}" NAME)

    if(NOT FILENAME STREQUAL "renderpass.schema.json")
        file(COPY "${file}" DESTINATION "${CMAKE_BINARY_DIR}/renderpasses")
    endif()
endforeach()

# Recursively find source and header files
file(GLOB_RECURSE SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "source/*.cpp" "source/*.c")
file(GLOB_RECURSE HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "include/*.hpp" "include/*.h")
file(GLOB_RECURSE SHADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "shaders/*.vert" "shaders/*.frag")
file(GLOB_RECURSE PIPELINES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "pipelines/*.json")
file(GLOB_RECURSE RENDERPASSES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "renderpasses/*.json")
file(GLOB_RECURSE FONTLOADERHEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "fontloader/include/*.h" "fontloader/include/*.hpp")
file(GLOB_RECURSE FONTLOADERSRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "fontloader/src/*.cpp" "fontloader/src/*.c")
file(GLOB_RECURSE DESCRIPTORSETS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "descriptorsets/*.json")
file(GLOB_RECURSE SCRIPTS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "scripts/*.ps1" "scripts/*.sh")

#want the main.cpp in the root
set(ROOT_SOURCES main.cpp)

#this will add the same directory structure we have in the github project
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${ROOT_SOURCES}) # no prefix we want it at the root
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/source" PREFIX "source" FILES ${SOURCES})   
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "include" FILES ${HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/fontloader/include" PREFIX "fontloader/include" FILES ${FONTLOADERHEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/fontloader/src" PREFIX "fontloader/source" FILES ${FONTLOADERSRC})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/shaders" PREFIX "shaders" FILES ${SHADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/pipelines" PREFIX "pipelines" FILES ${PIPELINES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/renderpasses" PREFIX "renderpasses" FILES ${RENDERPASSES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/descriptorsets" PREFIX "descriptorsets" FILES ${DESCRIPTORSETS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/scripts" PREFIX "scripts" FILES ${SCRIPTS})


add_executable(nova)
# Add both sources AND headers (optional but helps IDE indexing)
target_sources(nova PRIVATE 
    main.cpp  # explicitly add the root main.cpp file
    ${SOURCES}
    ${FONTLOADERSRC}
    ${HEADERS}          # add so ide can create directory strucure
    ${FONTLOADERHEADERS}  # add so ide can create directory strucure 
    ${SHADERS}         # Optional (shaders are also assets)
    ${PIPELINES}       # Adds pipeline JSON files to IDE
    ${RENDERPASSES}    # Adds renderpass JSON files to IDE
    ${DESCRIPTORSETS}  # Adds descriptor set JSON files to IDE
    ${SCRIPTS}         # adds scripts folder to the ide
)

target_include_directories(nova PRIVATE ${CMAKE_SOURCE_DIR}/include
                                        ${CMAKE_SOURCE_DIR}/fontloader/include
                                        ${Vulkan_INCLUDE_DIRS}
                                        ${FREETYPE_INCLUDE_DIRS}
                                        )

# Link libraries
target_link_libraries(nova
    PRIVATE
    Vulkan::Vulkan
    glfw
    ${FREETYPE_LIBRARIES}
)

# Post-build command to copy shaders and other files to output directory
add_custom_command(TARGET nova POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:nova>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/pipelines $<TARGET_FILE_DIR:nova>/pipelines
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/renderpasses $<TARGET_FILE_DIR:nova>/renderpasses
     COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/descriptorsets $<TARGET_FILE_DIR:nova>/descriptorsets
)