#!/bin/env sh

# Debug / Release
SUFFIX="-gcc"
CC=gcc cmake -S . -B build${SUFFIX} -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build${SUFFIX}
cmake --install build${SUFFIX} --prefix=install${SUFFIX}