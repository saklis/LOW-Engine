@echo off
setlocal

REM ===========================================
REM  LOW-Engine - Visual Studio 2022 (Win32)
REM  Build script
REM ===========================================

echo.
echo === Generating Visual Studio 2022 Win32 solution ===

REM Remove files from previous builds
if exist vs2022-win32-sln (
    echo Cleaning up files from previous generation...
    rmdir /s /q vs2022-win32-sln
)

REM Create build directory
mkdir vs2022-win32-sln

REM Generate VS solution file
cmake -S . -B vs2022-win32-sln -G "Visual Studio 17 2022" -A Win32

if errorlevel 1 (
    echo.
    echo !!! ERROR: Generation of VS solution file failed
    exit /b 1
)

echo.
echo === VS solution file generated! ===
echo Location: vs2022-win32-sln\LOW-Engine.sln
echo.

endlocal
pause
