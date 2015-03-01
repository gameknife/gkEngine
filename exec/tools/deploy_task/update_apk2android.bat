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

rem Step2 重新安装
%GKENGINE_HOME%\tools\adb install -r %GKENGINE_HOME%/android/bin/gklauncher-debug.apk

rem Step3 运行
rem %GKENGINE_HOME%\tools\adb shell am start -n com.kkstudio.gklauncher/android.app.NativeActivity
rem %GKENGINE_HOME%\tools\adb logcat gkENGINE:I *:S

pause