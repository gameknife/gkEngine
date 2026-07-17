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

if not exist %GKENGINE_HOME%\..\exec\bin32 mkdir %GKENGINE_HOME%\..\exec\bin32
if not exist %GKENGINE_HOME%\..\exec\bin64 mkdir %GKENGINE_HOME%\..\exec\bin64

copy /Y %GKENGINE_HOME%\..\code\thirdparty\dxsdk\bin\x86\* %GKENGINE_HOME%\..\exec\bin32
copy /Y %GKENGINE_HOME%\..\code\thirdparty\oglsdk\bin\* %GKENGINE_HOME%\..\exec\bin32
copy /Y %GKENGINE_HOME%\..\code\thirdparty\mscrt\x86\* %GKENGINE_HOME%\..\exec\bin32
copy /Y %GKENGINE_HOME%\..\code\thirdparty\dxsdk\bin\x64\* %GKENGINE_HOME%\..\exec\bin64
copy /Y %GKENGINE_HOME%\..\code\thirdparty\mscrt\x64\* %GKENGINE_HOME%\..\exec\bin64

rem texconv.exe is a 32-bit legacy tool. Keep its private runtime 32-bit too.
copy /Y %GKENGINE_HOME%\..\code\thirdparty\dxsdk\bin\x86\d3dx9_43.dll %GKENGINE_HOME%\..\exec\tools
copy /Y %GKENGINE_HOME%\..\code\thirdparty\dxsdk\bin\x86\D3DCompiler_43.dll %GKENGINE_HOME%\..\exec\tools
copy /Y %GKENGINE_HOME%\..\code\thirdparty\mscrt\x86\msvcp100.dll %GKENGINE_HOME%\..\exec\tools
copy /Y %GKENGINE_HOME%\..\code\thirdparty\mscrt\x86\msvcr100.dll %GKENGINE_HOME%\..\exec\tools
