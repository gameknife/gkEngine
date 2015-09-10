@echo off


echo .
echo .
echo -------------------------------------------------------------------------
echo This is the automatic deploy script for gkENGINE.
echo It will download dependency, medias packs from github, then unzip them.
echo Finally, it will try to build the whole Engine with MsBuild. And auto run the TestCase Demo.
echo -------------------------------------------------------------------------
echo .

echo .
echo .
echo First, please choose the optional media pack you want to install:
echo -------------------------------------------------------------------------
set pauseprogress=1

rem choose media first
set chosenmedia=1
CHOICE /C YN /M "Install conf_room demo?: Y:Install ; N:Skip"
IF %errorlevel%==1 set confroom=1
CHOICE /C YN /M "Install outdoor demo?: Y:Install ; N:Skip"
IF %errorlevel%==1 set outdoor=1
CHOICE /C YN /M "Install character demo?: Y:Install ; N:Skip"
IF %errorlevel%==1 set character=1

echo .
echo Next, All process will execute automaticly, just take seet and have a coffee.
echo -------------------------------------------------------------------------
pause


call init_engine_res.bat

cd exec\tools\global_task

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
echo MSBuild finded, build gkENGINE automaticly...
echo -------------------------------------------------------------------------
call %msBuildDir%\msbuild.exe %GKENGINE_HOME%\..\code\gkENGINE_CORE_Vc10.sln /p:Configuration=Develop /p:Platform=Win32
) ELSE (
echo MSBuild Can not find, you should build gkENGINE manmally.
echo -------------------------------------------------------------------------
)

rem strp5, auto run, if possible

IF EXIST %GKENGINE_HOME%\bin32\gkLauncherUniverse.exe (
echo Automatic launch the Testcase Demo...
echo -------------------------------------------------------------------------
call %GKENGINE_HOME%\bin32\gkLauncherUniverse.exe
) ELSE (
echo Build not success, faild to launch the Testcase Demo.
echo -------------------------------------------------------------------------
)

echo All thing done...

PAUSE