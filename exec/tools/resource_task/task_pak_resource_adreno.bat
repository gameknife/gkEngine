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
	
rem Step1 dd
call ..\resource_task\adreno_tga2atc_engine.bat
call ..\resource_task\adreno_tga2atc_media.bat

rem Step2 dd
%GKENGINE_HOME%\tools\gkPak -i "media\level" -o "paks\level.gpk" -l 5 -p 2
%GKENGINE_HOME%\tools\gkPak -i "media\objects" -o "paks\objects.gpk" -l 5 -p 2
%GKENGINE_HOME%\tools\gkPak -i "media\terrian" -o "paks\terrian.gpk" -l 5 -p 2
%GKENGINE_HOME%\tools\gkPak -i "media\textures" -o "paks\textures.gpk" -l 5 -p 2
%GKENGINE_HOME%\tools\gkPak -i "engine" -o "paks\engine.gpk" -l 5 -p 2
%GKENGINE_HOME%\tools\gkPak -i "media\config" -o "paks\config.gpk" -l 5 -p 2
%GKENGINE_HOME%\tools\gkPak -i "media\fonts" -o "paks\fonts.gpk" -l 5 -p 2

pause