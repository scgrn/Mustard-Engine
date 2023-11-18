@echo off

setlocal ENABLEEXTENSIONS

setlocal
for /f "delims=" %%x in (project.cfg) do (set "%%x")

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
    echo Building %PROJECT_NAME% in debug mode...

    md build\debug 2> nul
    cd build\debug
    cmake ..\.. -DCMAKE_BUILD_TYPE=Debug -DPROJECT_NAME=%PROJECT_NAME% -DMUSTARD_DIR=%MUSTARD_PATH% -DSDL2_DIR=%SDL2_PATH%
    cd ..\..
    cmake --build build\debug
exit /b 0

:buildRelease
    echo Building %PROJECT_NAME% in release mode...

    md build\release 2> nul
    cd build\release
    cmake ..\.. -DCMAKE_BUILD_TYPE=Release -DPROJECT_NAME=%PROJECT_NAME% -DMUSTARD_DIR=%MUSTARD_PATH% -DSDL2_DIR=%SDL2_PATH%
    cd ..\..
    cmake --build build\release
exit /b 0

:buildAssets
    echo Building asset archive...

    set MUSTARD_PATH=%MUSTARD_PATH:/=\%
    cd assets
    ..\%MUSTARD_PATH%\bin\Mustard-AssetCompiler ..\%PROJECT_NAME%.dat %ENCYPTION_KEY%
    cd ..
exit /b 0

:buildDist
    :: TODO: implement
exit /b 0

