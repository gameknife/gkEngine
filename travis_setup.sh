
# shadercache folder
source init_engine_res.sh

cd exec/tools/global_task

source set_global_env.sh

sh extract_thirdparty.sh

sh unzip_thirdparty.sh

#rem tex process
#cd exec/tools/resource_task
#call desktop_tga2dds_engine.bat
#sh sgx_tga2pvr_engine.sh
#call adreno_tga2atc_engine.bat

#PAUSE

