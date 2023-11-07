#!/bin/bash

mkdir build
mkdir build/debug
cd build/debug
cmake ../.. -D CMAKE_BUILD_TYPE=Debug
cd ../..
cmake --build build/debug

