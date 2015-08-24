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

IF NOT EXIST %GKENGINE_HOME%\media\terrian.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/media/terrian.7z
)

IF NOT EXIST %GKENGINE_HOME%\media\textures.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/media/textures.7z
)

IF NOT EXIST %GKENGINE_HOME%\media\textures.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/media/basic.7z
)

IF NOT EXIST %GKENGINE_HOME%\media\conf_room.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/media/conf_room.7z
)

%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\media\terrian.7z
%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\media\textures.7z
%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\media\basic.7z

%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\media\conf_room.7z

rem del %GKENGINE_HOME%\media\media.7z

pause