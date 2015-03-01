 @echo off
 	rem this file made by yikaiming 2012/12/13

    rem batch gammar reference
    
    rem echo %%~dpnA disk + dir + name
    rem echo %%~sA name+ext
    rem echo %%~nA name
    rem echo %%~fA abspathname
    rem echo %%~xA ext
    rem echo %%~pA dik

chcp 936
	
rem 根目录生成
set GKENGINE_HOME=%cd%\..\..\
echo 已设置根路径: %GKENGINE_HOME%

rem 版本号生成
set BUILD_NUMBER=1.0.0.%date:~5,2%%date:~8,2%
echo 已生成版本号: %BUILD_NUMBER%
