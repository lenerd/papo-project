include_directories (. ../src)

foreach (bench neuralnet backpropagation genetic)
    add_executable(bench_${bench} bench_${bench}.c)
    target_link_libraries(bench_${bench} ${PROJECT_NAME}_lib)
endforeach(bench)

configure_file (flops.py flops.py)
