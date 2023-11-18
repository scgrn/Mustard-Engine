#!/bin/bash

config=$1

function buildDebug() {
    echo "Building Mustard Engine in debug mode..."

    mkdir -p build/debug
    cd build/debug
    cmake ../.. -D CMAKE_BUILD_TYPE=Debug
    cd ../..
    cmake --build build/debug
}

function buildRelease() {
    echo "Building Mustard Engine in release mode..."

    mkdir -p build/release
    cd build/release
    cmake ../.. -D CMAKE_BUILD_TYPE=Release
    cd ../..
    cmake --build build/release
}

function buildAssetCompiler() {
    echo "Building asset compiler...."
    
    mkdir -p build/assetCompiler
    cd build/assetCompiler
    cmake ../../src/assetCompiler -D CMAKE_BUILD_TYPE=Release
    cd ../..
    cmake --build build/assetCompiler
}

function buildDocs() {
    echo "Building Lua API Documentation..."

    # TODO: implement
}

function buildTests() {
    echo "Building tests..."

    # TODO: implement
}

if [ "$config" == "debug" ]; then
    buildDebug
elif [ "$config" == "release" ]; then
    buildRelease
elif [ "$config" == "assetCompiler" ]; then
    buildAssetCompiler
elif [ "$config" == "docs" ]; then
    buildDocs
elif [ "$config" == "tests" ]; then
    buildTests
elif [ "$config" == "all" ]; then
    buildDebug
    buildRelease
    buildAssetCompiler
    buildDocs
    buildTests
else
    echo usage: ./build.sh config
    echo
    echo Where config is one of:
    echo
    echo debug
    echo release
    echo assetCompiler
    echo docs
    echo tests
    echo all
    echo
fi

