@ECHO OFF

SETLOCAL
SET ENCYPTION_KEY="Jordan, I SWEAR TO GOD!!"
SET PREMAKE_PATH=\premake\premake5
SET MUSTARD_PATH=\projects\Mustard

:: clean previous build artifacts
CLS
IF EXIST build (
    rd /S /Q build
)

%PREMAKE_PATH% gmake2

CD build
make verbose=1 %*
CD ..

CD assets
%MUSTARD_PATH%\bin\Mustard-AssetCompiler "..\Assets.dat" %ENCYPTION_KEY%
CD ..

ENDLOCAL

