#file(GLOB_RECURSE COMMON_SOURCES
#    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
#    "src/*.cpp" "src/*.c"
#)

file(GLOB_RECURSE COMMON_HEADERS
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "include/*.hpp" "include/*.h"
)
# Create a dummy target JUST for IDE to show headers in solution tree
add_custom_target(common_target SOURCES ${COMMON_HEADERS})

# Group headers in IDE under "include"
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "include" FILES ${COMMON_HEADERS})

add_library(common INTERFACE)
target_include_directories(common INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

