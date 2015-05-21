enable_testing ()

find_package (Check REQUIRED)
include_directories (${CHECK_INCLUDE_DIRS})
include_directories (. ../src)

set (LIBS ${LIBS} ${PROJECT_NAME}_lib ${CHECK_LIBRARIES})

# example test
add_executable (check_example check_example.c)
target_link_libraries (check_example ${LIBS})
add_test (check_example ${CMAKE_CURRENT_BINARY_DIR}/check_example)
