@ECHO OFF

:: HI, HERE'S A WINDOWS BATCH SCRIPT TO BUILD THE MUSTARD ENGINE

SETLOCAL
SET PREMAKE_PATH=\premake\premake5

:: clean previous build artifacts
CLS
IF EXIST build (
	rd /S /Q build
)
IF EXIST bin (
	rd /S /Q bin
)


:: touch
type nul >> .\src\core\version.h

:: TODO: get premake path
%PREMAKE_PATH% gmake2

:: TODO: determine make program by platform
CD build
make verbose=1 %*
CD ..


:: check premake exists
::where /q %PREMAKE_PATH%
::IF ERRORLEVEL 1 (
::    ECHO The application is missing. Ensure it is installed and placed in your PATH.
::    EXIT /B
::) ELSE (
::    ECHO Application exists. Let's go!
::)

::  build Lua API documentation
:: \lua51\lua \ldoc\ldoc.lua -p "Mustard Engine" -d docs src\script\
call genDocs.bat

mshta "javascript:code(close((V=(v=new ActiveXObject('SAPI.SpVoice')).GetVoices()).count&&v.Speak('ENGINE BUILD COMPLETE')))"


:: It is a deliberate misuse of the choice command, that Echo's nothing via a pipe to Choice,
:: causing a non-breaking error. STDERR is redirected to nul, and the default choice prompt
:: is suppressed via the /N switch, meaning no new line is output.
:: from https://stackoverflow.com/questions/27079534/how-to-start-a-system-beep-from-the-built-in-pc-speaker-using-a-batch-file/41107788#41107788

:: Echo/| CHOICE /N 2> nul & rem BEL

:: PAUSE

ENDLOCAL



