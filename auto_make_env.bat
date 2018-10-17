@echo off


echo .
echo .
echo -------------------------------------------------------------------------
echo This is the automatic deploy script for gkENGINE.
echo It will download dependency, medias packs from github, then unzip them.
echo Finally, it will try to build the whole Engine with MsBuild. And auto run the TestCase Demo.
echo -------------------------------------------------------------------------
echo .

call init_engine_res.bat

cd exec\tools\global_task

call set_global_env.bat

rem step01, download, extract, place the denpendencys

rem call extract_thirdparty.bat

rem step01-1, process default resource

call ..\resource_task\_obj2gmf_media.bat
call ..\resource_task\desktop_tga2dds_engine.bat

rem step02, download, extract, place the medias
call extract_media_pack.bat

echo All thing done...

PAUSE