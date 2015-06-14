find_package (Check)
if (CHECK_FOUND)

    enable_testing ()

    include_directories (${CHECK_INCLUDE_DIRS})
    include_directories (. ../src)

    set (LIBS ${LIBS} ${PROJECT_NAME}_lib ${CHECK_LIBRARIES})

    # example test
    add_executable (check_example check_example.c)
    target_link_libraries (check_example ${LIBS})
    add_test (check_example ${CMAKE_CURRENT_BINARY_DIR}/check_example)

    # go test
    add_executable (check_go check_go.c)
    target_link_libraries (check_go ${LIBS})
    add_test (check_go ${CMAKE_CURRENT_BINARY_DIR}/check_go)

    # math test
    add_executable (check_math check_math.c)
    target_link_libraries (check_math ${LIBS})
    add_test (check_math ${CMAKE_CURRENT_BINARY_DIR}/check_math)

    # neuralnet test
    add_executable (check_neuralnet check_neuralnet.c)
    target_link_libraries (check_neuralnet ${LIBS})
    add_test (check_neuralnet ${CMAKE_CURRENT_BINARY_DIR}/check_neuralnet)

    # feel free to add more tests :)

endif (CHECK_FOUND)
