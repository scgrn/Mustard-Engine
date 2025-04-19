cmake_minimum_required(VERSION 3.12.0)

project(Mustard-AssetCompiler VERSION 1.0.0)
set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../bin)

set(VENDOR_DIR "${PROJECT_SOURCE_DIR}/../vendor")

aux_source_directory("${VENDOR_DIR}/lua-5.3.5/src" LUA_SOURCE)
aux_source_directory("${VENDOR_DIR}/zlib-1.3.1" ZLIB_SOURCE)

list(REMOVE_ITEM LUA_SOURCE "${VENDOR_DIR}/lua-5.3.5/src/lua.c")
list(REMOVE_ITEM LUA_SOURCE "${VENDOR_DIR}/lua-5.3.5/src/luac.c")

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/assetCompiler.cpp
    ${ZLIB_SOURCE}
    ${LUA_SOURCE}
)

