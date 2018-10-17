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
call init_engine_res.bat

rem 2. global param setup
cd exec\tools\global_task
call set_global_env.bat

rem 3. process built-in resources
call ..\resource_task\_obj2gmf_media.bat
call ..\resource_task\desktop_tga2dds_engine.bat

rem 4. extract mediapack from submodule & process
call extract_media_pack.bat

echo All thing done...

PAUSE