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

rem Step1 trans dds
call ..\resource_task\_tga2dds_engine.bat
call ..\resource_task\_tga2dds_media.bat
call ..\resource_task\_obj2gmf_media.bat
call ..\resource_task\compile_all_shader.bat

rem Step2 pak
%GKENGINE_HOME%\tools\gkPak -i "media\level" -o "paks\level.gpk" -l 5 -p 0
%GKENGINE_HOME%\tools\gkPak -i "media\objects" -o "paks\objects.gpk" -l 5 -p 0
%GKENGINE_HOME%\tools\gkPak -i "media\terrian" -o "paks\terrian.gpk" -l 5 -p 0
%GKENGINE_HOME%\tools\gkPak -i "media\textures" -o "paks\textures.gpk" -l 5 -p 0
%GKENGINE_HOME%\tools\gkPak -i "engine" -o "paks\engine.gpk" -l 5 -p 0
%GKENGINE_HOME%\tools\gkPak -i "media\config" -o "paks\config.gpk" -l 5 -p 0
%GKENGINE_HOME%\tools\gkPak -i "media\fonts" -o "paks\fonts.gpk" -l 5 -p 0
