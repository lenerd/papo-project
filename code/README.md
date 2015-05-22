# Build Instructions

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    make
    make doc
    ctest
    ctest -v
