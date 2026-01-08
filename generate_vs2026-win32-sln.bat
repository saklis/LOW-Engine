@echo off
setlocal EnableDelayedExpansion

echo ===========================================
echo  LOW-Engine - Visual Studio 2026 (Win32)
echo  Generate / Build / Shortcut script
echo ===========================================

set "BUILD_DIR=build-vs2026-win32"
set "GENERATOR=Visual Studio 18 2026"
set "ARCH=Win32"
set "BUILD=0"

echo.
echo === Generating Visual Studio 2026 Win32 solution ===

REM run CMake
cmake -S . -B "%BUILD_DIR%" -G "%GENERATOR%" -A %ARCH%
if errorlevel 1 (
    echo.
    echo !!! ERROR: Solution generation failed
    pause
    exit /b 1
)

echo.
echo === Solution generation success ===

REM compile
if %BUILD% gtr 0 (
    cmake --build "%BUILD_DIR%" --config Debug
    if errorlevel 1 (
        echo.
        echo !!! ERROR: Compilation failed.
        pause
        exit /b 1
    )
) else (
    echo Compilation skipped [BUILD=%BUILD%]
)

echo.
echo === Creating link to .sln from "%BUILD_DIR%" ===

set "SLN_PATH="
for /f "delims=" %%f in ('dir /b /a:-d "%BUILD_DIR%\*.sln"') do (
    set "SLN_PATH=%CD%\%BUILD_DIR%\%%f"
    goto :found_sln
)

:found_sln
if not defined SLN_PATH (
    echo !!! ERROR: File .sln was not found in "%BUILD_DIR%".
    goto :end
)

echo File found: "%SLN_PATH%"

REM look for PowerShell
where powershell >nul 2>&1
if errorlevel 1 (
    echo.
    echo !!! ERROR: PowerShell is not present in PATH - can't create .lnk
    echo !!! Creation of .lnk skipped
    goto :end
)

REM create .lnk to solution file
set "LINK_DIR=%~dp0"
set "SLN_NAME="
for %%~ in ("%SLN_PATH%") do set "SLN_NAME=%~n0"
for %%A in ("%SLN_PATH%") do set "LINK_PATH=%LINK_DIR%%%~nA - VS2026 Win32.lnk"

echo.
echo === Creating shortcut ===
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$W=New-Object -ComObject WScript.Shell; " ^
  "$L=$W.CreateShortcut('%LINK_PATH%'); " ^
  "$L.TargetPath='%SLN_PATH%'; " ^
  "$L.WorkingDirectory='$(Split-Path -Parent ''%SLN_PATH%'')'; " ^
  "$L.IconLocation='%SLN_PATH%,0'; " ^
  "$L.Description='Open Visual Studio solution (Win32)'; " ^
  "$L.Save()"

if exist "%LINK_PATH%" (
    echo Shortcut created: "%LINK_PATH%"
) else (
    echo.
    echo !!! ERROR: Unable to create a shortcut: "%LINK_PATH%"
    pasue
)

:end
echo.
echo === FINSHED ===
echo.
endlocal
pause
