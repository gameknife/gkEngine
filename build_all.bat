@echo off

chcp 936

set msBuildDir=%WINDIR%\Microsoft.NET\Framework\v4.0.30319

IF EXIST %WINDIR%\Microsoft.NET\Framework\v4.0.30319\msbuild.exe (
echo MSBuild finded, build gkENGINE automaticly.
call %msBuildDir%\msbuild.exe gkENGINE_CORE_Vc10.sln /p:Configuration=Develop /p:Platform=Win32
) ELSE (
echo MSBuild Can not find, you should build gkENGINE manmally.
)


PAUSE