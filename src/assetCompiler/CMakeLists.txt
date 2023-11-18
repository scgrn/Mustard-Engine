cmake_minimum_required(VERSION 3.25.0)

project(Mustard-AssetCompiler VERSION 1.0.0)
set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../../bin)

set(VENDOR_DIR "${PROJECT_SOURCE_DIR}/../vendor")

# Collect third-party sources
aux_source_directory("${VENDOR_DIR}/tinyxml" TINYXML_SOURCE)
aux_source_directory("${VENDOR_DIR}/zlib-1.2.11" ZLIB_SOURCE)

list(REMOVE_ITEM TINYXML_SOURCE "${VENDOR_DIR}/tinyxml/xmltest.cpp")


add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/assetCompiler.cpp
    
    ${TINYXML_SOURCE}
    ${ZLIB_SOURCE}    
)

