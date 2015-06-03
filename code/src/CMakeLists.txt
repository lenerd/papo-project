# main source (startup functionality
set (MAIN_SOURCE main.c)

# other source files
set (PROJECT_SOURCES
    genetic/genetic_algorithm.c
    go/board.c
    go/game_controller.c
    go/record.c
    math_extend/math_ext.c
    neuralnet/neuralnet.c
    example/example.c
)

# Find includes in corresponding build directories
set (CMAKE_INCLUDE_CURRENT_DIR ON)

add_library (${PROJECT_NAME}_lib ${PROJECT_SOURCES})
add_executable (${PROJECT_NAME} ${MAIN_SOURCE})

target_link_libraries (${PROJECT_NAME} ${PROJECT_NAME}_lib)
