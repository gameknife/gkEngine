rem shadercache folder
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
call adreno_tga2atc_engine.bat

PAUSE