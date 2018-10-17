source set_global_env.sh
source common_func.sh

dir=$GKENGINE_HOME/media
if [ ! -d "$dir" ]
then
mkdir $GKENGINE_HOME/media
else
echo
fi
cd $GKENGINE_HOME/media

$GKENGINE_HOME/tools/7zr x -y $GKENGINE_HOME/../mediapack/base/basic.7z
$GKENGINE_HOME/tools/7zr x -y $GKENGINE_HOME/../mediapack/base/textures.7z
$GKENGINE_HOME/tools/7zr x -y $GKENGINE_HOME/../mediapack/confroom/conf_room.7z

cd $GKENGINE_HOME/tools/global_task

sh ../resource_task/_obj2gmf_media.sh
sh ../resource_task/osx_tga2dds_media.sh
sh ../resource_task/sgx_tga2pvr_media.sh

echo .
echo .
echo -------------------------------------------------------------------------
echo All thing done success.
echo -------------------------------------------------------------------------
echo .


#pause