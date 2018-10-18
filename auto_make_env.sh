echo .
echo .
echo -------------------------------------------------------------------------
echo "\033[36m This is the automatic deploy script for gkENGINE. \033[0m"
echo "\033[36m It will download dependency, medias packs from github, then unzip them. \033[0m"
echo "\033[36m Finally, it will try to build the whole Engine with MsBuild. And auto run the TestCase Demo. \033[0m"
echo -------------------------------------------------------------------------
echo .

# shadercache folder
mkdir_ifnotexsit(){
if [ ! -d "$1" ]; then
mkdir $1
fi
}

mkdir_ifnotexsit ./code/thirdparty
mkdir_ifnotexsit ./exec/media
mkdir_ifnotexsit ./exec/media/config
mkdir_ifnotexsit ./exec/paks

cp ./exec/tools/default_cfg/startup.cfg ./exec/media/config


cd exec/tools/global_task

source set_global_env.sh

echo .
echo "\033[36m [step 2] prepare the basic engine resources... \033[0m"
echo -------------------------------------------------------------------------

sh ../resource_task/_obj2gmf_media.sh
sh ../resource_task/osx_tga2dds_engine.sh
sh ../resource_task/sgx_tga2pvr_engine.sh

# media packs
source extract_media_pack.sh

sh ../resource_task/sgx_tga2pvr_media.sh
sh ../resource_task/task_pak_resource_ios.sh

