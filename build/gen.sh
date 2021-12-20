#!/bin/bash

CMAKE_BUILD_TYPE=Release
UNAME_ALL=$(uname -a)
SYSTEM=${UNAME_ALL%% *}

echo -e "\e[1;36m[ info ]: Current system: $SYSTEM\e[0m\n"

echo -e "\e[1;36m[ info ]: Using $(nproc) cores to build ...\e[0m"

if [[ $SYSTEM == "Darwin" ]]; then
  cmake -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -DCMAKE_C_COMPILER=/opt/homebrew/Cellar/gcc/11.2.0_3/bin/gcc-11 -DCMAKE_CXX_COMPILER=/opt/homebrew/Cellar/gcc/11.2.0_3/bin/g++-11 ..
  cmake --build . --config $CMAKE_BUILD_TYPE -j"$(nproc)"
fi

echo -e "\e[1;36m[ info ]: Build finished ...\e[0m\n"

echo -e "\e[1;36m[ info ]: Start CTest ...\e[0m"
ctest -C $CMAKE_BUILD_TYPE

echo -e "\n\e[1;36m[ info ]: CTest finished ...\e[0m"
