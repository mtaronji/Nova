
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)

#BUILDING NOVA
# Recursively find source and header files
file(GLOB_RECURSE NOVA_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cpp" "src/*.c")
file(GLOB_RECURSE NOVA_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}" "include/*.hpp" "include/*.h")

set(TEST_NOVA "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

#ide construction
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "src" FILES ${NOVA_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "include" FILES ${NOVA_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"  FILES ${TEST_NOVA})

# the following files are in the root source cmakelists.txt. They are needed for nova to function
#therefore they must be copied for all 
file(GLOB_RECURSE SHADERS CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "shaders/*.vert" "shaders/*.frag" "shaders/*.txt")  # .txt is for cmake
file(GLOB_RECURSE PIPELINES CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "pipelines/*.json")
file(GLOB_RECURSE RENDERPASSES CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "renderpasses/*.json")
file(GLOB_RECURSE FONTLOADERHEADERS CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "fontloader/include/*.h" "fontloader/include/*.hpp")
file(GLOB_RECURSE FONTLOADERSRC CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "fontloader/src/*.cpp" "fontloader/src/*.c")
file(GLOB_RECURSE DESCRIPTORS CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "descriptors/*.json")
file(GLOB_RECURSE SCRIPTS CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "scripts/*.ps1" "scripts/*.sh")
file(GLOB_RECURSE CONFIG CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "config/*.json")
file(GLOB_RECURSE CONFIG_GENERATION CONFIGURE_DEPENDS  "${CMAKE_SOURCE_DIR}" "generated/*.hpp" "generated/*.in")


add_library(nova STATIC)

# Add both sources AND headers (for IDE indexing)
target_sources(nova PRIVATE 
    ${NOVA_SOURCES}
    ${NOVA_HEADERS}            # add so ide can create directory strucure
)

#PUBLIC SO OTHER CODE CAN SEE THE HEADER
target_include_directories(nova PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link libraries
target_link_libraries(nova
    PUBLIC
    Vulkan::Vulkan
    glfw
    common
)

#THEY ARE PUBLIC SO THE MACROS COULD GET DEFINED IN CODE THAT USES NOVA
if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config generators like Visual Studio
    target_compile_definitions(nova PUBLIC
        $<$<CONFIG:Debug>:OUTPUT_DIRECTORY=\"${CMAKE_SOURCE_DIR}/build/x64/$<CONFIG>\">
        $<$<CONFIG:Release>:OUTPUT_DIRECTORY=\"${CMAKE_SOURCE_DIR}/build/x64/$<CONFIG>\">
)
 
else()
    # Single-config generators like Make/Ninja on Linux
    target_compile_definitions(nova PUBLIC
        $<$<CONFIG:Debug>:OUTPUT_DIRECTORY=\"${CMAKE_SOURCE_DIR}/build/$<CONFIG>\">
        $<$<CONFIG:Release>:OUTPUT_DIRECTORY=\"${CMAKE_SOURCE_DIR}/build/$<CONFIG>\">
)
endif()

#for testing
set(SHADERS_INPUT_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADERS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/compiled_shaders")

nova_compile_shaders(${SHADERS_INPUT_DIR} ${SHADERS_OUTPUT_DIR})

add_executable(test_nova main.cpp)
add_dependencies(test_nova compile_shaders_target)

# Optional: copy shaders next to test_nova binary
add_custom_command(TARGET test_nova POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${SHADERS_OUTPUT_DIR}" "$<TARGET_FILE_DIR:test_nova>/shaders"
)

# Post-build command to copy shaders and other files to output directory
add_custom_command(TARGET test_nova POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SHADERS_OUTPUT_DIR}" "$<TARGET_FILE_DIR:test_nova>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/pipelines" "$<TARGET_FILE_DIR:test_nova>/pipelines"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/renderpasses" "$<TARGET_FILE_DIR:test_nova>/renderpasses"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/descriptors" "$<TARGET_FILE_DIR:test_nova>/descriptors"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/config" "$<TARGET_FILE_DIR:test_nova>/config"
)

target_sources(test_nova PRIVATE ${TEST_NOVA})
target_link_libraries(test_nova PRIVATE nova)

