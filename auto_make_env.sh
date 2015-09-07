echo .
echo .
echo -------------------------------------------------------------------------
echo "\033[36m This is the automatic deploy script for gkENGINE. \033[0m"
echo "\033[36m It will download dependency, medias packs from github, then unzip them. \033[0m"
echo "\033[36m Finally, it will try to build the whole Engine with MsBuild. And auto run the TestCase Demo. \033[0m"
echo -------------------------------------------------------------------------
echo .

# shadercache folder
source init_engine_res.sh
cd exec/tools/global_task

autoinstall=1

install_conf_room=0
install_outdoor=0
install_character=0

echo "\033[33m Do you want install conf_room demo? y/n: \033[0m"
read var
if  [ $var = "y" ]
then
install_conf_room=1
fi

echo "\033[33m Do you want install outdoor demo? y/n: \033[0m"
read var
if  [ $var = "y" ]
then
install_outdoor=1
fi

echo "\033[33m Do you want install character demo? y/n: \033[0m"
read var
if  [ $var = "y" ]
then
install_character=1
fi

echo .
echo "\033[36m Next, All process will execute automaticly, just take seet and have a coffee. \033[0m"
echo -------------------------------------------------------------------------
read -n1 -p "Press any key to continue..."

#exit 0

source set_global_env.sh


echo .
echo "\033[36m [step 1] download & extract dependecies... \033[0m"
echo -------------------------------------------------------------------------
sh extract_thirdparty.sh

source verify_thirdparty.sh

if [ $verifyresult = 0 ]
then
echo -------------------------------------------------------------------------
echo "\033[31m dependency archive file download unsuccessful, the auto deploy must shutup. Please check your network. and try again. \033[0m"
echo "\033[31m [Optinal choice of bad network with github: download dependency archives manmally, and run this script again.] \033[0m"
echo -------------------------------------------------------------------------
read -n1 -p "Press any key to exit..."
exit 0
fi

sh unzip_thirdparty.sh


echo .
echo "\033[36m [step 2] prepare the basic engine resources... \033[0m"
echo -------------------------------------------------------------------------

sh ../resource_task/_obj2gmf_media.sh
sh ../resource_task/osx_tga2dds_engine.sh
sh ../resource_task/sgx_tga2pvr_engine.sh



# media packs

source extract_media_pack.sh

sh ../resource_task/sgx_tga2pvr_media.sh

# pak

sh ../resource_task/task_pak_resource_ios.sh

# build gkENGINE
xcodebuild -workspace $GKENGINE_HOME/../code/engine/solutions/gkENGINE_xcode.xcworkspace -scheme release_osx

# run gkENGINE
open -n $GKENGINE_HOME/ios/Release-OSX/gkLauncherMac.app


#rem tex process
#cd exec/tools/resource_task
#call desktop_tga2dds_engine.bat
#sh sgx_tga2pvr_engine.sh
#call adreno_tga2atc_engine.bat

#PAUSE

