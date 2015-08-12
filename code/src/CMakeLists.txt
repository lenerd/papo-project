# other source files
set (PROJECT_SOURCES
    genetic/genetic_algorithm.c
    go/board.c
    go/game_controller.c
    go/record.c
    util/util.c
    util/math_ext.c
    neuralnet/neuralnet.c
    neuralnet/learning.c
    training/training.c
    example/example.c
)

# Find includes in corresponding build directories
set (CMAKE_INCLUDE_CURRENT_DIR ON)

add_library (${PROJECT_NAME}_lib ${PROJECT_SOURCES})
target_link_libraries (${PROJECT_NAME}_lib m)

# executables
add_executable (${PROJECT_NAME} exe/main.c)
target_link_libraries (${PROJECT_NAME} ${PROJECT_NAME}_lib)

add_executable (ngg_tool exe/ngg_tool.c exe/ngg_tool_impl.c)
target_link_libraries (ngg_tool ${PROJECT_NAME}_lib)
