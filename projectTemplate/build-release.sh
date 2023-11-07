#!/bin/bash

mkdir build
mkdir build/release
cd build/release
cmake ../.. -D CMAKE_BUILD_TYPE=Release
cd ../..
cmake --build build/release

