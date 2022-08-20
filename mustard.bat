@echo off

IF "%~1" == "" GOTO PrintHelp
IF "%~1" == "create" GOTO NewProject
IF "%~1" == "compile" GOTO Compile

vendor\premake5.exe %1
GOTO Done

:PrintHelp

echo.
echo Enter 'mustard.cmd action' where action is one of the following:
echo.
echo   create [path]     Create a new project
echo   compile           Will generate make file then compile using the make file.
echo   clean             Remove all binaries and intermediate binaries and project files.
echo   codelite          Generate CodeLite project files
echo   gmake             Generate GNU makefiles for Linux
echo   vs2005            Generate Visual Studio 2005 project files
echo   vs2008            Generate Visual Studio 2008 project files
echo   vs2010            Generate Visual Studio 2010 project files
echo   vs2012            Generate Visual Studio 2012 project files
echo   vs2013            Generate Visual Studio 2013 project files
echo   vs2015            Generate Visual Studio 2015 project files
echo   vs2017            Generate Visual Studio 2017 project files
echo   vs2019            Generate Visual Studio 2019 project files
echo   xcode4            Generate Apple Xcode 4 project files
echo.

GOTO Done

:NewProject
cls
set /p AB_PROJECT_NAME="ENTER PROJECT NAME: "

:: copy project template to new project folder
xcopy /E /S projectTemplate "%AB_PROJECTS_ROOT%\%AB_PROJECT_NAME%\"
pushd "%AB_PROJECTS_ROOT%\%AB_PROJECT_NAME%\"

:: prepend window title as project name in interface.lua
pushd assets\scripts\
echo local TITLE = "%AB_PROJECT_NAME%" > temp.lua
echo. >> temp.lua
type main.lua >> temp.lua
type temp.lua > main.lua
del temp.lua
popd

:: create run.bat
echo bin\debug\"%AB_PROJECT_NAME%" > run.bat

GOTO Done

:Compile

:Done

