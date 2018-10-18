@echo off

echo .
echo .
echo -------------------------------------------------------------------------
echo This is the automatic deploy script for gkENGINE.
echo It will download dependency, medias packs from github, then unzip them.
echo Finally, it will try to build the whole Engine with MsBuild. And auto run the TestCase Demo.
echo -------------------------------------------------------------------------
echo .

rem 1. process built-in resources

mkdir exec\engine\shaders\d3d9\shadercache
mkdir code\thirdparty
mkdir exec\media
mkdir exec\paks

rem copy cfg
mkdir exec\media\config
copy exec\tools\default_cfg\startup.cfg exec\media\config

rem tex process
cd exec\tools\resource_task
call desktop_tga2dds_engine.bat
call sgx_tga2pvr_engine.bat
cd ..\..\..\
rem call adreno_tga2atc_engine.bat

rem 2. global param setup
cd exec\tools\global_task
call set_global_env.bat

rem 3. process built-in resources
call ..\resource_task\_obj2gmf_media.bat
call ..\resource_task\desktop_tga2dds_engine.bat

rem 4. extract mediapack from submodule & process
call extract_media_pack.bat

echo All thing done...
