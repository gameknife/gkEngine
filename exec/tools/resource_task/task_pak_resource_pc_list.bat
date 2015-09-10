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

rem Step2 pak
%GKENGINE_HOME%\tools\gkPak -i "paks\engine.gpk" -o "123" -l 5 -p 0
pause
