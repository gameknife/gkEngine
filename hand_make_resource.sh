cd exec/tools/global_task
sh unzip_media_pack.sh


cd ../resource_task
sh _obj2gmf_media.sh
sh osx_tga2dds_media.sh
sh sgx_tga2pvr_media.sh
#call adreno_tga2atc_media.bat

#pause
