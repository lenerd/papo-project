include_directories (. ../src)

foreach (experiment xor)
    add_executable(exp_${experiment} exp_${experiment}.c)
    target_link_libraries(exp_${experiment} ${PROJECT_NAME}_lib)
endforeach(experiment)
