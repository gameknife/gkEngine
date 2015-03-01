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

mkdir %GKENGINE_HOME%\..\exec\bin32

copy %GKENGINE_HOME%\..\code\thirdparty\ToolkitPro_15_01\bin\vc100\ToolkitPro1501vc100U.dll %GKENGINE_HOME%\..\exec\bin32
copy %GKENGINE_HOME%\..\code\thirdparty\ToolkitPro_15_01\bin\vc100\ToolkitPro1501vc100UD.dll %GKENGINE_HOME%\..\exec\bin32
copy %GKENGINE_HOME%\..\code\thirdparty\dxsdk\bin\* %GKENGINE_HOME%\..\exec\bin32
copy %GKENGINE_HOME%\..\code\thirdparty\oglsdk\bin\* %GKENGINE_HOME%\..\exec\bin32
copy %GKENGINE_HOME%\..\code\thirdparty\mscrt\* %GKENGINE_HOME%\..\exec\bin32

pause