@echo off

:: remove previous build artifacts
rd /S /Q bin 2> nul
rd /S /Q build 2> nul

md build
cd build
cmake ..
cd..
cmake --build build

:: generate Lua API documentation
echo Building Lua API Documentation...
\lua51\lua \ldoc\ldoc.lua -o "index" -p "Mustard Engine" -d docs src\main\script\
