
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)

#BUILDING NOVA
# Recursively find source and header files
file(GLOB_RECURSE NOVA_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE NOVA_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE IMAGE_PROCESSING_HEADERS CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}/image/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/image/include/*.hpp")

set(TEST_NOVA "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

#ide construction
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "" FILES ${NOVA_SOURCES} ${NOVA_HEADERS} ${IMAGE_PROCESSING_HEADERS} ${TEST_NOVA})

add_library(nova STATIC)

# Add both sources AND headers (for IDE indexing)
target_sources(nova PRIVATE 
    ${NOVA_SOURCES}
    ${NOVA_HEADERS}            # add so ide can create directory strucure
    ${IMAGE_PROCESSING_HEADERS}
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

#PUBLIC SO OTHER CODE CAN SEE THE HEADER
target_include_directories(nova PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/image/include)

# Link libraries
target_link_libraries(nova
    PUBLIC
    Vulkan::Vulkan
    glfw
    common
)


#for testing
set(SHADERS_INPUT_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADERS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/compiled_shaders")

#COMPILE SOME SHADERS FOR NOVA AND MAKE NOVA DEPEND ON TEHJM
set (NOVA_SHADER_TARGET "NOVA_SHADER_TARGET")
nova_compile_shaders(${NOVA_SHADER_TARGET} ${SHADERS_INPUT_DIR} ${SHADERS_OUTPUT_DIR})

add_executable(test_nova main.cpp)
add_dependencies(test_nova ${NOVA_SHADER_TARGET})

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
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/textures" "$<TARGET_FILE_DIR:test_nova>/textures"
)

target_sources(test_nova PRIVATE ${TEST_NOVA})
target_link_libraries(test_nova PRIVATE nova)

