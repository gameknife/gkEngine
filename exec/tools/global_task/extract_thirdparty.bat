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

IF NOT EXIST %GKENGINE_HOME%\..\code\thirdparty mkdir %GKENGINE_HOME%\..\code\thirdparty

cd %GKENGINE_HOME%\..\code\thirdparty

echo Start Downloading dependency win32 from github...

IF NOT EXIST %GKENGINE_HOME%\..\code\thirdparty\dxsdk.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/dependency/dxsdk.7z
)
IF NOT EXIST %GKENGINE_HOME%\..\code\thirdparty\freetype.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/dependency/freetype.7z
)
IF NOT EXIST %GKENGINE_HOME%\..\code\thirdparty\misc.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/dependency/misc.7z
)
IF NOT EXIST %GKENGINE_HOME%\..\code\thirdparty\mscrt.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/dependency/mscrt.7z
)
IF NOT EXIST %GKENGINE_HOME%\..\code\thirdparty\oglsdk.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/dependency/oglsdk.7z
)

echo Download Success! Extracting...

cd %restore%

call unzip_thirdparty.bat

call place_thirdparty.bat

echo All denpendency OK.

rem https://raw.githubusercontent.com/gameKnife/gkengine-resource/dependency/dxsdk.7z
rem %GKENGINE_HOME%\tools\7zr x %GKENGINE_HOME%\..\code\thirdparty\depends.7z

rem del %GKENGINE_HOME%\..\code\thirdparty\depends.7z