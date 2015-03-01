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
	
del %GKENGINE_HOME%\..\INTERMEDIATE\* /Q /S /F
del %GKENGINE_HOME%\..\*.pdb /Q /S /F
del %GKENGINE_HOME%\..\*.ilk /Q /S /F
del %GKENGINE_HOME%\..\*.user /Q /S /F
del %GKENGINE_HOME%\..\*.ipch /Q /S /F
del %GKENGINE_HOME%\..\*.sdf /Q /S /F
del %GKENGINE_HOME%\..\*.aps /Q /S /F
rmdir %GKENGINE_HOME%\..\code\Lib /Q /S
pause