@echo off
for /R hwcode %%A in (*.fx) do (
     rem echo %%~dpnA
     rem echo %%~sA
     rem echo %%~nA
     "fxc.exe" /Zpc /T fx_2_0 /Fo %%~nA.o %%~dpnA.fx
)

pause