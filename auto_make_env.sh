# shadercache folder
cd exec/tools/global_task

echo .
echo .
echo -------------------------------------------------------------------------
echo This is the automatic deploy script for gkENGINE.
echo It will download dependency, medias packs from github, then unzip them.
echo Finally, it will try to build the whole Engine with MsBuild. And auto run the TestCase Demo.
echo -------------------------------------------------------------------------
echo .


autoinstall=1

install_conf_room=0
install_outdoor=0
install_character=0

echo "Do you want install conf_room demo? y/n:"
read var
if  [ $var = "y" ]
then
install_conf_room=1
fi

echo "Do you want install outdoor demo? y/n:"
read var
if  [ $var = "y" ]
then
install_outdoor=1
fi

echo "Do you want install character demo? y/n:"
read var
if  [ $var = "y" ]
then
install_character=1
fi

echo .
echo Next, All process will execute automaticly, just take seet and have a coffee.
echo -------------------------------------------------------------------------
read -n1 -p "Press any key to continue..."

source set_global_env.sh

sh extract_thirdparty.sh
sh unzip_thirdparty.sh

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

