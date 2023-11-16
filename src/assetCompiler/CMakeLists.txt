cmake_minimum_required(VERSION 3.25.0)

project(Mustard-AssetCompiler VERSION 1.0.0)
set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/assetCompiler.cpp
)

