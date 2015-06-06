cmake_minimum_required (VERSION 3.0)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# project name
set (PROJECT_NAME neugengo)
project (${PROJECT_NAME})

# version numbers
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)


# compile flags
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -Wextra -pedantic -Wconversion")
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb -O0")
set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")


# build project
add_subdirectory (src)

# build tests
enable_testing ()
add_subdirectory (tests)

# documentation
find_package (Doxygen)
if (DOXYGEN_FOUND)
    configure_file (
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY
    )
    add_custom_target (
        doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif (DOXYGEN_FOUND)
