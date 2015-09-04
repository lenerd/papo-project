# other source files
set (PROJECT_SOURCES
    genetic/genetic_algorithm.c
    go/board.c
    # go/game_controller.c
    go/player.c
    go/game.c
    go/record.c
    util/util.c
    util/math_ext.c
    util/mpi.c
    util/scheduler.c
    neuralnet/neuralnet.c
    neuralnet/learning.c
    training/training.c
)

# Find includes in corresponding build directories
set (CMAKE_INCLUDE_CURRENT_DIR ON)

add_library (${PROJECT_NAME}_lib ${PROJECT_SOURCES})
target_link_libraries (${PROJECT_NAME}_lib m rt readline)
target_link_libraries (${PROJECT_NAME}_lib ${MPI_C_LIBRARIES})

# executables
# add_executable (${PROJECT_NAME} exe/main.c)
# target_link_libraries (${PROJECT_NAME} ${PROJECT_NAME}_lib)

add_executable (ngg_tool exe/ngg_tool.c exe/ngg_tool_impl.c)
target_link_libraries (ngg_tool ${PROJECT_NAME}_lib)

add_executable (ngg_game exe/ngg_game.c exe/ngg_game_impl.c exe/ngg_game_helper.c)
target_link_libraries (ngg_game ${PROJECT_NAME}_lib)

add_executable (ngg_hvsai exe/ngg_hvsai.c)
target_link_libraries (ngg_hvsai ${PROJECT_NAME}_lib)

add_executable(learning_success exe/learning_success.c)
target_link_libraries (learning_success ${PROJECT_NAME}_lib)
