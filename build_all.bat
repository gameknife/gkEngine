
@echo off
rem msbuild gkENGINE_CORE_Vc10.sln /t:rebuild
rem SBuild.exe C:\Folder Containing My Visual Studio Solution\My Solution.sln /property:Configuration=Develop

set msBuildDir=%WINDIR%\Microsoft.NET\Framework\v4.0.30319
REM perhaps you need a different framework? %WINDIR%\Microsoft.NET\Framework\v4.0.30319
rem call %msBuildDir%\msbuild.exe /help
call %msBuildDir%\msbuild.exe gkENGINE_CORE_Vc10.sln /p:Configuration=Develop /p:Platform=Win32
rem /target:AllTargetsWrapper "MySolutionOrCsProj.sln" /p:Configuration=Debug;FavoriteFood=Twix /l:FileLogger,Microsoft.Build.Engine;logfile=ZZZZZMSBuildSetupAndBuildAllTargetsWrapper_Debug.log
PAUSE