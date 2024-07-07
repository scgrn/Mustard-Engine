#!/bin/bash

config=$1
source project.cfg

function buildDebug() {
    echo "Building $PROJECT_NAME in debug mode..."

    mkdir -p build/debug
    cd build/debug
    cmake ../.. -DCMAKE_BUILD_TYPE=Debug -DPROJECT_NAME=$PROJECT_NAME -DMUSTARD_DIR=$MUSTARD_PATH -DSDL2_DIR=$SDL2_PATH
    cd ../..
    cmake --build build/debug
}

function buildRelease() {
    echo "Building $PROJECT_NAME in release mode..."

    mkdir -p build/release
    cd build/release
    cmake ../.. -DCMAKE_BUILD_TYPE=Release -DPROJECT_NAME=$PROJECT_NAME -DMUSTARD_DIR=$MUSTARD_PATH -DSDL2_DIR=$SDL2_PATH
    cd ../..
    cmake --build build/release
}

function buildAssets() {
    echo "Building asset archive..."

    cd assets
    ../$MUSTARD_PATH/bin/Mustard-AssetCompiler ../$PROJECT_NAME.dat "$ENCYPTION_KEY"
    cd ..
}

function buildDist() {
    echo "Building $PROJECT_NAME for distribution..."
    
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

