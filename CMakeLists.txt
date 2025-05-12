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



file(COPY ${CMAKE_SOURCE_DIR}/Pipelines DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/RenderPasses DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})

add_executable(nova 
    main.cpp 
    source/Nova.cpp
    source/Shell.cpp 
    source/VulkanEngine.cpp 
    source/GPU.cpp 
    source/Shader.cpp 
    source/RenderPassManager.cpp
    source/FramebufferManager.cpp
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