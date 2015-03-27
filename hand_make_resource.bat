cd exec\tools\global_task
call unzip_media_pack.bat
cd ..\resource_task
call _obj2gmf_media.bat
call desktop_tga2dds_media.bat
call sgx_tga2pvr_media.bat
call adreno_tga2atc_media.bat

pause