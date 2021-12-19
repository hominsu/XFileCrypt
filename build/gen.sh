#!/bin/sh

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/opt/homebrew/Cellar/gcc/11.2.0_2/bin/gcc-11 -DCMAKE_CXX_COMPILER=/opt/homebrew/Cellar/gcc/11.2.0_2/bin/g++-11 ..

printf "\nUsing %d cores to build ...\n\n" "$(nproc)"

cmake --build . -j"$(nproc)"
printf "\nBuild finished ...\n\n"
