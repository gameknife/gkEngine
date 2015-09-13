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

IF NOT EXIST %GKENGINE_HOME%\media\basic.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/media/basic.7z
)

%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\media\terrian.7z
%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\media\textures.7z
%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\media\basic.7z

echo .
echo .
echo -------------------------------------------------------------------------
echo Default Media Pack Download and Extracted. 
echo Now choose optional media pack.
echo -------------------------------------------------------------------------
echo .

IF "%chosenmedia%"=="1" (
goto :AUTOINSTALL 
) ELSE (
goto :MANMALINSTALL
)
:AUTOINSTALL

IF "%confroom%"=="1" (
IF NOT EXIST %GKENGINE_HOME%\media\conf_room.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/media/conf_room.7z
)

%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\media\conf_room.7z
)

IF "%outdoor%"=="1" (
IF NOT EXIST %GKENGINE_HOME%\media\outdoor.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/media/outdoor.7z
)

%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\media\outdoor.7z
)

IF "%character%"=="1" (
IF NOT EXIST %GKENGINE_HOME%\media\character.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/media/character.7z
)

%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\media\character.7z
)

goto :POSTPROCESS

:MANMALINSTALL

CHOICE /C YN /M "Install conf_room demo?: Y:Install ; N:Skip"
IF %errorlevel%==1 goto :CONFROOM
goto :CONFROOMEND
:CONFROOM
IF NOT EXIST %GKENGINE_HOME%\media\conf_room.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/media/conf_room.7z
)
%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\media\conf_room.7z
:CONFROOMEND
CHOICE /C YN /M "Install out_door demo?: Y; N"
IF %errorlevel%==1 goto :OUTDOOR
goto :OUTDOOREND
:OUTDOOR
IF NOT EXIST %GKENGINE_HOME%\media\outdoor.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/media/outdoor.7z
)
%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\media\outdoor.7z
:OUTDOOREND
CHOICE /C YN /M "Install character demo?: Y; N"
IF %errorlevel%==1 goto :CHARA
goto :CHARAEND
:CHARA
IF NOT EXIST %GKENGINE_HOME%\media\character.7z (
%GKENGINE_HOME%\tools\wget --no-check-certificate https://raw.githubusercontent.com/gameknife/gkengine-resource/master/media/character.7z
)
%GKENGINE_HOME%\tools\7zr x -y %GKENGINE_HOME%\media\character.7z
:CHARAEND

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


