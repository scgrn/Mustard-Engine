@echo off

if not exist bin\Mustard-Debug.lib (
	cls
	echo BUILDING MUSTARD ENGINE
	echo. 
	echo THIS MAY TAKE A COUPLE MINUTES...
	start /wait cmd /c build.cmd
)

:: check project root is set
IF "%AB_PROJECTS_ROOT%"=="" (
	cls
	set /p AB_PROJECTS_ROOT="ENTER PROJECTS ROOT: "
)

:: check if project name is set
IF NOT "%AB_PROJECT_NAME%"=="" (
	goto PROJECT_MENU
)

cls
:MAIN_MENU
echo MUSTARD ENGINE
echo.
echo  [1] NEW PROJECT
echo  [2] LIST PROJECTS
echo  [3] LOAD PROJECT
echo  [4] EXIT
echo.

choice /c:1234 /n /m "SELECT: "
if %errorlevel% == 1 goto NEW_PROJECT
if %errorlevel% == 2 goto LIST_PROJECTS
if %errorlevel% == 3 goto LOAD_PROJECT
goto EXIT

:NEW_PROJECT
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

goto BUILD

goto PROJECT_MENU

:LIST_PROJECTS
cls
pushd "%AB_PROJECTS_ROOT%
dir /b /ad /p
echo.
popd
pause
cls
goto MAIN_MENU

:LOAD_PROJECT
cls
set /p AB_PROJECT_NAME="ENTER PROJECT NAME: "
:: TODO: check project folder exists
:: https://stackoverflow.com/questions/21033801/checking-if-a-folder-exists-using-a-bat-file
pushd "%AB_PROJECTS_ROOT%\%AB_PROJECT_NAME%\"
goto PROJECT_MENU

:PROJECT_MENU
cls
echo %AB_PROJECT_NAME% PROJECT MENU:
echo.
echo  [1] RUN
echo  [2] EDIT SOURCES
echo  [3] REBUILD
echo  [4] COMPILE AND LINK UPDATED SOURCES
echo  [5] BUILD RELEASE MODE
echo  [6] PACKAGE FOR DISTRIBUTION
echo  [7] OPEN COMMAND WINDOW
echo  [8] OPEN PROJECT FOLDER
echo  [9] QUIT
echo.

choice /c:123456789 /n /m "SELECT: "
if %ERRORLEVEL% == 1 goto RUN
if %ERRORLEVEL% == 2 goto EDIT
if %ERRORLEVEL% == 3 goto BUILD
if %ERRORLEVEL% == 4 goto COMPILE
if %ERRORLEVEL% == 5 goto BUILD_RELEASE
if %ERRORLEVEL% == 6 goto PACKAGE
if %ERRORLEVEL% == 7 goto OPEN_CMD
if %ERRORLEVEL% == 8 goto OPEN_FOLDER
popd
cls
goto MAIN_MENU

:RUN
cls
echo RUNNING %AB_PROJECT_NAME%...
start /wait cmd /c run.bat
goto PROJECT_MENU

:EDIT
cls
echo NOT IMPLEMENTED
echo.
pause
goto PROJECT_MENU

:BUILD
cls
echo BUILDING %AB_PROJECT_NAME%...
start /wait cmd /c build.cmd
goto PROJECT_MENU

:COMPILE
cls
echo BUILDING %AB_PROJECT_NAME%...
start /wait cmd /c compile.bat
goto PROJECT_MENU

:BUILD_RELEASE
cls
echo NOT IMPLEMENTED
echo.
pause
goto PROJECT_MENU

:PACKAGE
cls
echo NOT IMPLEMENTED
echo.
pause
goto PROJECT_MENU

:OPEN_CMD
cls
start cmd
goto PROJECT_MENU

:OPEN_FOLDER
start .
goto PROJECT_MENU

:EXIT
set AB_PROJECT_NAME=
cls

