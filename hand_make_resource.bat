cd exec\tools\global_task
call unzip_media_pack.bat
cd ..\resource_task
call _obj2gmf_media.bat
call _tga2dds_media.bat
call _tga2pvr_media.bat

pause