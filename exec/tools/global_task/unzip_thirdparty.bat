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

set restore=%cd%

mkdir %GKENGINE_HOME%\..\code\thirdparty
cd %GKENGINE_HOME%\..\code\thirdparty
%GKENGINE_HOME%\tools\7zr x %GKENGINE_HOME%\..\code\thirdparty\depends.7z
cd %restore%
del %GKENGINE_HOME%\..\code\thirdparty\depends.7z
