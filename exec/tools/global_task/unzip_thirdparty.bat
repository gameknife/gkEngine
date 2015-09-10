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
%GKENGINE_HOME%\tools\7zr x -y -bd %GKENGINE_HOME%\..\code\thirdparty\dxsdk.7z
%GKENGINE_HOME%\tools\7zr x -y -bd  %GKENGINE_HOME%\..\code\thirdparty\freetype.7z
%GKENGINE_HOME%\tools\7zr x -y -bd  %GKENGINE_HOME%\..\code\thirdparty\misc.7z
%GKENGINE_HOME%\tools\7zr x -y -bd  %GKENGINE_HOME%\..\code\thirdparty\oglsdk.7z
%GKENGINE_HOME%\tools\7zr x -y -bd  %GKENGINE_HOME%\..\code\thirdparty\toolkitpro_15.7z
%GKENGINE_HOME%\tools\7zr x -y -bd  %GKENGINE_HOME%\..\code\thirdparty\havoksdk.7z
%GKENGINE_HOME%\tools\7zr x -y -bd  %GKENGINE_HOME%\..\code\thirdparty\havoksdk_win32.7z
%GKENGINE_HOME%\tools\7zr x -y -bd  %GKENGINE_HOME%\..\code\thirdparty\mscrt.7z

cd %restore%
rem del %GKENGINE_HOME%\..\code\thirdparty\depends.7z
