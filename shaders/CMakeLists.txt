
find_program(GLSLANG_VALIDATOR glslangValidator REQUIRED)

function(nova_compile_shaders SHADER_INPUT_DIR SHADER_OUTPUT_DIR)
  
    if(NOT GLSLANG_VALIDATOR)
        message(FATAL_ERROR "glslangValidator not found in PATH")
    endif()

    file(GLOB_RECURSE SHADERS CONFIGURE_DEPENDS
        "${SHADER_INPUT_DIR}/*.vert"
        "${SHADER_INPUT_DIR}/*.frag"
        "${SHADER_INPUT_DIR}/*.comp"
        "${SHADER_INPUT_DIR}/*.geom"
    )

    if(SHADERS STREQUAL "")
        message(WARNING "No shaders found in ${SHADER_INPUT_DIR}")
        return()
    endif()

    file(MAKE_DIRECTORY "${SHADER_OUTPUT_DIR}")

    set(SPIRV_FILES "")
    foreach(SHADER ${SHADERS})
        get_filename_component(FILE_NAME ${SHADER} NAME)
        set(SPIRV "${SHADER_OUTPUT_DIR}/${FILE_NAME}.spv")

        add_custom_command(
            OUTPUT "${SPIRV}"
            COMMAND ${CMAKE_COMMAND} -E echo "Compiling shader: ${SHADER}"
            COMMAND ${GLSLANG_VALIDATOR} -V "${SHADER}" -o "${SPIRV}"
            DEPENDS "${SHADER}"
            BYPRODUCTS "${SPIRV}"
            COMMENT "Compiling ${FILE_NAME}"
            VERBATIM
        )

        list(APPEND SPIRV_FILES "${SPIRV}")
    endforeach()

    add_custom_target(compile_shaders_target ALL DEPENDS ${SPIRV_FILES})
endfunction()