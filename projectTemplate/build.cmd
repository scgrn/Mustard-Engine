@echo off

setlocal ENABLEEXTENSIONS

setlocal
set config=%1

if "%config%" == "debug" (
    call:buildDebug
) else if "%config%" == "release" (
    call:buildRelease
) else if "%config%" == "assets" (
    call:buildAssets
) else if "%config%" == "dist" (
    call:buildDist
) else if "%config%" == "all" (
    call:buildDebug
    call:buildRelease
    call:buildAssets
) else (
    echo usage: build config
    echo.
    echo Where config is one of:
    echo.
    echo debug
    echo release
    echo assets
    echo dist
    echo all
)

endlocal
exit /b 0

:buildDebug
    echo Building project in debug mode...

    md build\debug 2> nul
    cd build\debug
    cmake ..\.. -D CMAKE_BUILD_TYPE=Debug
    cd ..\..
    cmake --build build\debug    
exit /b 0

:buildRelease
    echo Building project in release mode...

    md build\release 2> nul
    cd build\release
    cmake ..\.. -D CMAKE_BUILD_TYPE=Release
    cd ..\..
    cmake --build build\release    
exit /b 0

:buildAssets
    echo Building asset archive...

    SET ENCYPTION_KEY="Jordan, I SWEAR TO GOD!!"
    SET MUSTARD_PATH=..\Mustard

    CD assets
    %MUSTARD_PATH%\bin\Mustard-AssetCompiler "..\Assets.dat" %ENCYPTION_KEY%
    CD ..
exit /b 0

:buildDist
    :: TODO: implement
exit /b 0

