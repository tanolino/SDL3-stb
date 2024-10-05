#!/bin/env sh

# Debug / Release
SUFFIX="-gcc"
CC=gcc
CXX=g++
cmake -S . -B build${SUFFIX} -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build${SUFFIX}
cmake --install build${SUFFIX} --prefix=install${SUFFIX}
