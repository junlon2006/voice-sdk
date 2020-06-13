#!/bin/bash
cd build
rm -rf *
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake .. -DX86=ON
make -j8