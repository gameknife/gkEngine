 @echo off
 	rem this file made by yikaiming 2012/12/13

    rem batch gammar reference
    
    rem echo %%~dpnA disk + dir + name
    rem echo %%~sA name+ext
    rem echo %%~nA name
    rem echo %%~fA abspathname
    rem echo %%~xA ext
    rem echo %%~pA dir

call ..\global_task\set_global_env.bat

for /R %GKENGINE_HOME%\media %%A in (*.tga) do (
    
    IF NOT EXIST %%~dpnA.dds (
    	echo [convert file] "%%~nxA" to "%%~nA.dds"
	%GKENGINE_HOME%\tools\texconv -nologo -f DXT5 -ft dds -o %%~dpA %%~fA
    ) ELSE (
		echo -nologo -f DXT5 -ft dds -o %%~dpA %%~fA
    	echo [file exist] %%~nA.dds
    )

)

for /R %GKENGINE_HOME%\media %%A in (*.jpg) do (
    
    IF NOT EXIST %%~dpnA.dds (
    	echo [convert file] "%%~nxA" to "%%~nA.dds"
	%GKENGINE_HOME%\tools\texconv -nologo -f DXT5 -ft dds -o %%~dpA %%~fA
    ) ELSE (
		echo -nologo -f DXT5 -ft dds -o %%~dpA %%~fA
    	echo [file exist] %%~nA.dds
    )

)

rem pause