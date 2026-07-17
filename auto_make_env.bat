@echo off
setlocal EnableExtensions
set "GKENGINE_ROOT=%~dp0"
pushd "%GKENGINE_ROOT%"

where git >nul 2>&1
if errorlevel 1 (
    echo [ERROR] git is required.
    popd
    endlocal
    exit /b 1
)

echo Initializing git submodules...
git submodule update --init --recursive
if errorlevel 1 (
    echo [ERROR] Failed to initialize git submodules.
    popd
    endlocal
    exit /b 1
)

echo .
echo .
echo -------------------------------------------------------------------------
echo This is the automatic deploy script for gkENGINE.
echo It will download dependency, medias packs from github, then unzip them.
echo Finally, it will try to build the whole Engine with MsBuild. And auto run the TestCase Demo.
echo -------------------------------------------------------------------------
echo .

rem 1. process built-in resources

if not exist exec\engine\shaders\d3d9\shadercache mkdir exec\engine\shaders\d3d9\shadercache
if not exist exec\media mkdir exec\media
if not exist exec\paks mkdir exec\paks

rem copy cfg
if not exist exec\media\config mkdir exec\media\config
copy /Y exec\tools\default_cfg\startup.cfg exec\media\config\startup.cfg >nul

if not exist code\thirdparty\mscrt\x86\msvcr100.dll (
    echo [ERROR] x86 MSVC runtime is missing from code\thirdparty.
    popd
    endlocal
    exit /b 1
)

rem Deploy third-party files. This also repairs the 32-bit texconv runtime.
pushd exec\tools\global_task
call place_thirdparty.bat
if errorlevel 1 (
    echo [ERROR] Failed to deploy third-party runtime files.
    popd
    popd
    endlocal
    exit /b 1
)
popd

rem tex process
pushd exec\tools\resource_task
call desktop_tga2dds_engine.bat
call sgx_tga2pvr_engine.bat
if errorlevel 1 (
    echo [ERROR] Failed to process engine resources.
    popd
    popd
    endlocal
    exit /b 1
)
popd
rem call adreno_tga2atc_engine.bat

rem 2. global param setup
pushd exec\tools\global_task
call set_global_env.bat

rem 3. process built-in resources
call ..\resource_task\_obj2gmf_media.bat
call ..\resource_task\desktop_tga2dds_engine.bat
if errorlevel 1 (
    echo [ERROR] Failed to process built-in resources.
    popd
    popd
    endlocal
    exit /b 1
)

rem 4. extract mediapack from submodule & process
call extract_media_pack.bat
if errorlevel 1 (
    echo [ERROR] Failed to extract media packs.
    popd
    popd
    endlocal
    exit /b 1
)
popd

echo All thing done...
popd
endlocal
exit /b 0
