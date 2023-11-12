@echo off

setlocal ENABLEEXTENSIONS

setlocal
for /f "delims=" %%x in (project.cfg) do (set "%%x")

set config=%1
if "%config%" == "debug" (
    bin\%PROJECT_NAME%-Debug.exe
) else if "%config%" == "release" (
    bin\%PROJECT_NAME%-Release.exe
) else (
    echo usage: run config
    echo.
    echo Where config is one of:
    echo.
    echo debug
    echo release
)

endlocal
exit /b 0

