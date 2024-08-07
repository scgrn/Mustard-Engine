#!/bin/bash

set -e
config=$1

function buildDebug() {
    echo "Building Mustard Engine in debug mode..."

    mkdir -p build/debug
    cd build/debug
    cmake ../../src/platform/desktop -D CMAKE_BUILD_TYPE=Debug
    cd ../..
    cmake --build build/debug
}

function buildRelease() {
    echo "Building Mustard Engine in release mode..."

    mkdir -p build/release
    cd build/release
    cmake ../../src/platform/desktop -D CMAKE_BUILD_TYPE=Release
    cd ../..
    cmake --build build/release
}

function buildWebDebug() {
    if [[ -z "${EMCMAKE}" ]]; then
        source ~/emsdk/emsdk_env.sh
    fi

    echo "Building Mustard Engine for web in debug mode..."

    mkdir -p build/web-debug
    cd build/web-debug
    emcmake cmake ../../src/platform/web -D CMAKE_BUILD_TYPE=Debug
    cd ../..
    cmake --build build/web-debug
}

function buildWebRelease() {
    if [[ -z "${EMCMAKE}" ]]; then
        source ~/emsdk/emsdk_env.sh
    fi

    echo "Building Mustard Engine for web in release mode..."

    mkdir -p build/web-release
    cd build/web-release
    emcmake cmake ../../src/platform/web -D CMAKE_BUILD_TYPE=Release
    cd ../..
    cmake --build build/web-release
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

    ldoc -o index -p "Mustard Engine" -d docs/luaAPI ./src/main/script/
    cd docs
    doxygen Doxyfile
    cd ..
}

function buildTests() {
    echo "Building tests..."

    # TODO: implement
}

if [ "$config" == "debug" ]; then
    buildDebug
elif [ "$config" == "release" ]; then
    buildRelease
elif [ "$config" == "web-debug" ]; then
    buildWebDebug
elif [ "$config" == "web-release" ]; then
    buildWebRelease
elif [ "$config" == "assetCompiler" ]; then
    buildAssetCompiler
elif [ "$config" == "docs" ]; then
    buildDocs
elif [ "$config" == "tests" ]; then
    buildTests
elif [ "$config" == "all" ]; then
    buildDebug
    buildRelease
    buildWebDebug
    buildWebRelease
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
    echo web-debug
    echo web-release
    echo assetCompiler
    echo docs
    echo tests
    echo all
    echo
fi

