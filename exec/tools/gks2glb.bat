@echo off
setlocal
python "%~dp0gks2glb.py" %*
exit /b %errorlevel%
