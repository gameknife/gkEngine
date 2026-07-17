
@echo off
setlocal EnableExtensions
set "GKENGINE_ROOT=%~dp0"

cmake -S "%GKENGINE_ROOT%." -B "%GKENGINE_ROOT%build-win64" -A x64
set "EXIT_CODE=%ERRORLEVEL%"
endlocal & exit /b %EXIT_CODE%

