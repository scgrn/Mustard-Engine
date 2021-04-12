@ECHO OFF

: HI, HERE'S A WINDOWS BATCH SCRIPT TO BUILD A MUSTARD GAME

IF "%AB_PROJECT_NAME%"=="" (
	echo ENVIRONMENT VARIABLE AB_PROJECT_NAME NOT SET. ABORTING.
	pause
	exit
)

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
%MUSTARD_PATH%\bin\ABAC "..\%AB_PROJECT_NAME%.dat" %ENCYPTION_KEY%
CD ..

::mshta "javascript:code(close((V=(v=new ActiveXObject('SAPI.SpVoice')).GetVoices()).count&&v.Speak('PROJECT BUILD COMPLETE')))"
::PAUSE

ENDLOCAL

