 @echo off
 	rem this file made by yikaiming 2012/12/13

    rem batch gammar reference
    
    rem echo %%~dpnA disk + dir + name
    rem echo %%~sA name+ext
    rem echo %%~nA name
    rem echo %%~fA abspathname
    rem echo %%~xA ext
    rem echo %%~pA dik

rem global env
call ..\global_task\set_global_env.bat

set FOLDER_NAME=android_%BUILD_NUMBER%
echo %FOLDER_NAME%	

rem Step3 ÔËÐÐ
%GKENGINE_HOME%\tools\adb logcat -c
%GKENGINE_HOME%\tools\adb shell am start -n com.kkstudio.gklauncher/android.app.NativeActivity
%GKENGINE_HOME%\tools\adb logcat gkENGINE:I *:S

pause