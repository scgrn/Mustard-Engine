#!/bin/bash

config=$1

function buildDebug() {
    echo "Building project in debug mode..."

    mkdir -p build/debug
    cd build/debug
    cmake ../.. -D CMAKE_BUILD_TYPE=Debug
    cd ../..
    cmake --build build/debug
}

function buildRelease() {
    echo "Building project in release mode..."

    mkdir -p build/release
    cd build/release
    cmake ../.. -D CMAKE_BUILD_TYPE=Release
    cd ../..
    cmake --build build/release
}

function buildAssets() {
    echo "Building asset archive..."

    # TODO: implement
}

function buildDist() {
    buildAssets
    buildRelease
    
    # TODO: implement
}

if [ "$config" == "debug" ]; then
    buildDebug
elif [ "$config" == "release" ]; then
    buildRelease
elif [ "$config" == "assets" ]; then
    buildAssets
elif [ "$config" == "dist" ]; then
    buildDist
elif [ "$config" == "all" ]; then
    buildDebug
    buildRelease
    buildAssets
else
    echo usage: ./build.sh config
    echo
    echo Where config is one of:
    echo
    echo debug
    echo release
    echo assets
    echo dist
    echo all
    echo
fi

