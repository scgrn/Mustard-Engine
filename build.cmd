@echo off

setlocal ENABLEEXTENSIONS

setlocal
set config=%1

if "%config%" == "debug" (
    call:buildDebug
) else if "%config%" == "release" (
    call:buildRelease
) else if "%config%" == "web" (
    call:buildWeb
) else if "%config%" == "assetCompiler" (
    call:buildAssetCompiler
) else if "%config%" == "docs" (
    call:buildDocs
) else if "%config%" == "tests" (
    call:buildTests
) else if "%config%" == "all" (
    call:buildDebug
    call:buildRelease
    call:buildWeb
    call:buildAssetCompiler
    call:buildDocs
    call:buildTests
) else (
    echo usage: build config
    echo.
    echo Where config is one of:
    echo.
    echo debug
    echo release
    echo web
    echo assetCompiler
    echo docs
    echo tests
    echo all
)

endlocal
exit /b 0

:buildDebug
    echo Building Mustard Engine in debug mode...

    md build\debug 2> nul
    cd build\debug
    cmake ..\.. -D CMAKE_BUILD_TYPE=Debug
    cd ..\..
    cmake --build build\debug    
exit /b 0

:buildRelease
    echo Building Mustard Engine in release mode...

    md build\release 2> nul
    cd build\release
    cmake ..\.. -D CMAKE_BUILD_TYPE=Release
    cd ..\..
    cmake --build build\release    
exit /b 0

:buildWeb
    echo Building Mustard Engine for web...

    md build\web 2> nul
    cd build\web
    :: TODO: set output name, ie libMustard-Web.a
    call emcmake cmake ..\.. -D CMAKE_BUILD_TYPE=Release
    cd ..\..
    cmake --build build\web 
exit /b 0

:buildAssetCompiler
    echo Building asset compiler...

    md build\assetCompiler 2> nul
    cd build\assetCompiler
    cmake ..\..\src\assetCompiler -D CMAKE_BUILD_TYPE=Release
    cd ..\..
    cmake --build build\assetCompiler
exit /b 0

:buildDocs
    echo Building Lua API Documentation...

    :: TODO: make this portable
    \lua51\lua \ldoc\ldoc.lua -o "index" -p "Mustard Engine" -d docs src\main\script\
exit /b 0

:buildTests
    echo Building tests...

    :: TODO: implement
exit /b 0
