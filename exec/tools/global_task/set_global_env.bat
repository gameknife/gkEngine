 @echo off
 	rem this file made by yikaiming 2012/12/13

    rem batch gammar reference
    
    rem echo %%~dpnA disk + dir + name
    rem echo %%~sA name+ext
    rem echo %%~nA name
    rem echo %%~fA abspathname
    rem echo %%~xA ext
    rem echo %%~pA dik
	
rem root path setted
set GKENGINE_HOME=%cd%\..\..\
echo root path setted: %GKENGINE_HOME%

rem root version setted
set BUILD_NUMBER=1.0.0.%date:~5,2%%date:~8,2%
echo root version setted: %BUILD_NUMBER%
