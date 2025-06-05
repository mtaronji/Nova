cmake_minimum_required(VERSION 3.13)  # CMake version check
project(nova)               # Create project "magus"
set(CMAKE_CXX_STANDARD 23)         # Enable c++23 standard


# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Set compiler flags for each build type
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

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
file(GLOB_RECURSE SOURCES "source/*.cpp" "source/*.c")
file(GLOB_RECURSE HEADERS "include/*.hpp" "include/*.h")
file(GLOB_RECURSE SHADERS "shaders/*.vert" "shaders/*.frag")
file(GLOB_RECURSE PIPELINES "Pipelines/*.json")
file(GLOB_RECURSE RENDERPASSES "RenderPasses/*.json")
file(GLOB_RECURSE FONTLOADERHEADERS "fontloader/include/*.h" "fontloader/include/*.hpp")
file(GLOB_RECURSE FONTLOADERSRC "fontloader/src/*.cpp" "fontloader/src/*.c")
file(GLOB_RECURSE DESCRIPTORSETS "descriptorsets/*.json")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/source" FILES ${SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" FILES ${HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/fontloader/include" FILES ${FONTLOADERHEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/fontloader/src" FILES ${FONTLOADERSRC})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/shaders" FILES ${SHADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/pipelines" FILES ${PIPELINES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/renderpasses" FILES ${RENDERPASSES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/descriptorsets" FILES ${DESCRIPTORSETS})


add_executable(nova 
    main.cpp 
    source/Nova.cpp
    source/Shell.cpp 
    source/VulkanEngine.cpp 
    source/GPU.cpp 
    source/Shader.cpp 
    source/RenderPassManager.cpp
    source/FramebufferGenerator.cpp
    source/PipelineManager.cpp
    source/CommandManager.cpp
    source/SwapchainManager.cpp 
    source/SyncManager.cpp
    source/Renderer.cpp 
    source/DescriptorAllocator.cpp
    source/GraphicsPipelineLoader.cpp 
    source/RenderpassLoader.cpp
    source/DescriptorAllocator.cpp
    source/Threadpool.cpp
    source/BufferOps.cpp
    source/BufferResource.cpp
    source/Mesh.cpp
    source/ImageManager.cpp
    source/PipelineLibrary.cpp
    source/RenderpassLibrary.cpp
    source/FramebufferLibrary.cpp
    source/ResourceManager.cpp
    source/DescriptorsetLoader.cpp
)
target_include_directories(nova PRIVATE ${CMAKE_SOURCE_DIR}/Include
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