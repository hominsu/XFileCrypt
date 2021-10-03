#!/bin/sh

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/opt/homebrew/Cellar/gcc/11.2.0/bin/gcc-11 -DCMAKE_CXX_COMPILER=/opt/homebrew/Cellar/gcc/11.2.0/bin/g++-11 ..

cores_num=$(grep -c processor /proc/cpuinfo)

printf "\nUsing %d cores to build ...\n\n" "$cores_num"

cmake --build . -j"$cores_num"
printf "\nBuild finished ...\n\n"
