cmake_minimum_required (VERSION 2.8)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# project name
set (PROJECT_NAME neugengo)
project (${PROJECT_NAME})

# version numbers
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)


# compile flags
set (CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -ggdb3 -std=gnu11 -pedantic -Wall -Wextra -Wconversion -Wdouble-promotion -Wfloat-equal"
)
# set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
# set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
set (CMAKE_C_FLAGS_DEBUG "-O0")
set (CMAKE_C_FLAGS_RELEASE "-O3")

# mpi
find_package (MPI REQUIRED)
include_directories (${MPI_INCLUDE_PATH})

if (VAMPIR)
    message ("Building with VampirTrace support.")
    set (CMAKE_C_FLAGS "-vt:cc ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS}")
    set (CMAKE_C_COMPILER "vtcc")
endif (VAMPIR)

# openmp
find_package (OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif (OPENMP_FOUND)

# build project
add_subdirectory (src)

# build tests
enable_testing ()
add_subdirectory (tests)

# build benchmarks
add_subdirectory (bench)

# build experiments
add_subdirectory (experiments)

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
