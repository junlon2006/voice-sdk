#!/bin/bash
curPath=$(readlink -f "$(dirname "$0")")

cd build
rm -rf *
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake .. -DX86=ON -DUNI_VOICE_SDK_3RD_PORTAUDIO=$curPath/3rd/portaudio
make -j8