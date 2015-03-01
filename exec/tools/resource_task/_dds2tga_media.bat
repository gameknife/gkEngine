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

for /R %GKENGINE_HOME%\media %%A in (*.dds) do (
    
    IF NOT EXIST %%~dpnA.tga (
    	echo [convert file] %%~nxA to %%~nA.tga
	%GKENGINE_HOME%\tools\texconv -nologo -ft tga -o %%~dpA %%~fA
    ) ELSE (
    	echo [file exist] %%~nA.tga
    )

)

rem pause