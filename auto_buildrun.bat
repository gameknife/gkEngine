@echo off
setlocal EnableExtensions
set "GKENGINE_ROOT=%~dp0"

if not exist "%GKENGINE_ROOT%build-win64\CMakeCache.txt" (
    echo [ERROR] build-win64 is not configured. Run auto_cmake.bat first.
    endlocal
    exit /b 1
)

taskkill /IM gkLauncher.exe /T /F >nul 2>&1

cmake --build "%GKENGINE_ROOT%build-win64" --config RelWithDebInfo --target ALL_BUILD --parallel
if errorlevel 1 (
    echo [ERROR] Build failed.
    endlocal
    exit /b 1
)

if not exist "%GKENGINE_ROOT%exec\bin64\gkLauncher.exe" (
    echo [ERROR] gkLauncher.exe was not produced in exec\bin64.
    endlocal
    exit /b 1
)

echo Starting gkLauncher with conf_room...
start "gkLauncher" /D "%GKENGINE_ROOT%exec\bin64" "%GKENGINE_ROOT%exec\bin64\gkLauncher.exe"
endlocal
exit /b 0
