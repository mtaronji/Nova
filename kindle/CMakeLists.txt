
file(GLOB_RECURSE KINDLE_SOURCES
     CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}"
    "src/*.cpp" "src/*.c"
)
file(GLOB_RECURSE KINDLE_HEADERS
     CONFIGURE_DEPENDS  "${CMAKE_CURRENT_SOURCE_DIR}"
    "include/*.hpp" "include/*.h"
)

set(TEST_KINDLE "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "src" FILES ${KINDLE_SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/include" PREFIX "include" FILES ${KINDLE_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"  FILES ${TEST_KINDLE})

add_library(kindle STATIC
    ${KINDLE_SOURCES}
    ${KINDLE_HEADERS}  # for IDEs
)

target_include_directories(kindle PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(kindle PRIVATE common)

add_executable(test_kindle main.cpp)
target_link_libraries(test_kindle PRIVATE kindle)

