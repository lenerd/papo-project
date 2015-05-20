cmake_minimum_required (VERSION 3.2)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# project name
set (PROJECT_NAME neugengo)
project (${PROJECT_NAME})

# version numbers
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)


# compile flags
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -Wextra -pedantic")
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb -O0")
set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")


# build project
add_subdirectory (src)

# build tests
enable_testing ()
add_subdirectory (tests)

# documentation
