# shadercache folder
mkdir exec/engine/shaders/d3d9/shadercache
mkdir code/thirdparty
mkdir exec/media

#rem copy cfg
mkdir exec/media/config
cp exec/tools/default_cfg/startup.cfg exec/media/config

#rem tex process
cd exec/tools/resource_task
#call desktop_tga2dds_engine.bat
sh sgx_tga2pvr_engine.sh
#call adreno_tga2atc_engine.bat

#PAUSE

