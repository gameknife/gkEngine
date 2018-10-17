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

IF NOT EXIST %GKENGINE_HOME%\media mkdir %GKENGINE_HOME%\media

cd %GKENGINE_HOME%\media

%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\..\mediapack\base\textures.7z
%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\..\mediapack\base\basic.7z

echo .
echo .
echo -------------------------------------------------------------------------
echo Default Media Pack Download and Extracted. 
echo Now choose optional media pack.
echo -------------------------------------------------------------------------
echo .

%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\..\mediapack\confroom\conf_room.7z

goto :POSTPROCESS

:POSTPROCESS

echo .
echo .
echo -------------------------------------------------------------------------
echo Optinal media pack download and extracted. 
echo Now transform to windows dds format....
echo -------------------------------------------------------------------------
echo .

cd %restore%

call ..\resource_task\_obj2gmf_media.bat
call ..\resource_task\desktop_tga2dds_media.bat

rem del %GKENGINE_HOME%\media\media.7z
echo .
echo .
echo -------------------------------------------------------------------------
echo All thing done success. 
echo -------------------------------------------------------------------------
echo .

IF "%pauseprogress%"=="1" (
echo auto close
) ELSE (
pause
)


