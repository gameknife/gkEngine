 @echo off
 	rem this file made by yikaiming 2012/12/13

    rem batch gammar reference
    
    rem echo %%~dpnA 			disk + dir + name
    rem echo %%~sA 				name+ext
    rem echo %%~nA 				name
    rem echo %%~fA 				abspathname
    rem echo %%~xA 				ext
    rem echo %%~pA 				dik
	
	rem %date:~4,0%				YEAR
	rem %date:~5,2%				MONTH
	rem %date:~8,2%				DAY

rem global env
call ..\global_task\set_global_env.bat

rem Step1 Pak
call ..\resource_task\task_pak_resource_android.bat

rem Step2 生成目标
set FOLDER_NAME=android_%BUILD_NUMBER%
echo %FOLDER_NAME%	

xcopy %GKENGINE_HOME%\bin32\*.dll %GKENGINE_HOME%\builds\%FOLDER_NAME%\bin32\ /Y
xcopy %GKENGINE_HOME%\bin32\*.exe %GKENGINE_HOME%\builds\%FOLDER_NAME%\bin32\ /Y
xcopy %GKENGINE_HOME%\paks\*.gpk %GKENGINE_HOME%\builds\%FOLDER_NAME%\paks\ /Y
xcopy %GKENGINE_HOME%\android\bin\*.apk %GKENGINE_HOME%\builds\%FOLDER_NAME%\apk\ /Y
xcopy %GKENGINE_HOME%\*.* %GKENGINE_HOME%\builds\%FOLDER_NAME%\ /Y

rem Step3 再次打压缩包
rem 7z
%GKENGINE_HOME%\tools\7zr a -mmt=4 -mx=9 %GKENGINE_HOME%\builds\%FOLDER_NAME%.7z %GKENGINE_HOME%\builds\%FOLDER_NAME%\ 

pause