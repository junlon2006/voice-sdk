#!/bin/bash
cd build
rm -rf *
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake .. -DX86=ON -DUNI_VOICE_SDK_3RD_PORTAUDIO=/media/psf/Home/Desktop/workspace/github/junlon2006/voice-sdk/3rd/portaudio
make -j8