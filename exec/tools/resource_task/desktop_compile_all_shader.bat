 @echo off
 	rem this file made by yikaiming 2012/12/13

    rem batch gammar reference
    
    rem echo %%~dpnA disk + dir + name
    rem echo %%~sA name+ext
    rem echo %%~nA name
    rem echo %%~fA abspathname
    rem echo %%~xA ext
    rem echo %%~pA dik

call ..\global_task\set_global_env.bat

for /R %GKENGINE_HOME%\engine\shaders\template %%A in (*.gfx) do (
    
    echo [compile shader] %%~nxA
    %GKENGINE_HOME%\tools\gkResourceCompiler -i %%~fA -o %%~dpnA.o -l 5 -p 0-i -y 0

)

rem pause