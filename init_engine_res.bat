rem shadercache folder
mkdir exec\engine\shaders\d3d9\shadercache
mkdir code\thirdparty
mkdir exec\media

rem copy cfg
mkdir exec\media\config
copy exec\tools\default_cfg\startup.cfg exec\media\config

rem tex process
cd exec\tools\resource_task
call _tga2dds_engine.bat
call _tga2pvr_engine.bat

PAUSE