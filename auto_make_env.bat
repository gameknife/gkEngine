cd exec\tools\global_task

set pauseprogress=1

call set_global_env.bat

rem step01, download, extract, place the denpendencys

call extract_thirdparty.bat

rem step01-1, process default resource

call ..\resource_task\_obj2gmf_media.bat
call ..\resource_task\desktop_tga2dds_engine.bat

rem step02, download, extract, place the medias

call extract_media_pack.bat

rem step4, auto build, if possible

set msBuildDir=%WINDIR%\Microsoft.NET\Framework\v4.0.30319

IF EXIST %WINDIR%\Microsoft.NET\Framework\v4.0.30319\msbuild.exe (
echo MSBuild finded, build gkENGINE automaticly.
call %msBuildDir%\msbuild.exe %GKENGINE_HOME%\..\gkENGINE_CORE_Vc10.sln /p:Configuration=Develop /p:Platform=Win32
) ELSE (
echo MSBuild Can not find, you should build gkENGINE manmally.
)

rem strp5, auto run, if possible

IF EXIST %GKENGINE_HOME%\bin32\gkLauncherUniverse.exe (
call %GKENGINE_HOME%\bin32\gkLauncherUniverse.exe
)

echo Press Any Key to Continue...

PAUSE