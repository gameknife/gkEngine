cd exec\tools\global_task

call set_global_env.bat

set msBuildDir="%programfiles(x86)%\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\amd64"

IF EXIST "%programfiles(x86)%\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\amd64\msbuild.exe" (
echo MSBuild finded, build gkENGINE automaticly...
echo -------------------------------------------------------------------------
call %msBuildDir%\msbuild.exe %GKENGINE_HOME%\..\code\gkENGINE_Vc14.sln /p:Configuration=Develop_Latest /p:Platform=x64
) ELSE (
echo MSBuild Can not find, you should build gkENGINE manmally.
echo -------------------------------------------------------------------------
)