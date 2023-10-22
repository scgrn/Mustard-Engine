@ECHO OFF

:: remove previous build artifacts
RD /S /Q bin 2> nul
RD /S /Q build 2> nul

MD build
CD build
cmake ..
CD..
cmake --build build

SETLOCAL
SET ENCYPTION_KEY="Jordan, I SWEAR TO GOD!!"
SET MUSTARD_PATH=..\Mustard

CD assets
%MUSTARD_PATH%\bin\Mustard-AssetCompiler "..\Assets.dat" %ENCYPTION_KEY%
CD ..

ENDLOCAL
