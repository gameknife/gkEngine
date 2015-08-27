source set_global_env.sh

dir=$GKENGINE_HOME/media
if [ ! -d "$dir" ]
then
mkdir $GKENGINE_HOME/media
else
echo
fi
cd $GKENGINE_HOME/media

downloadmedia(){
file=$GKENGINE_HOME/media/$1.7z
if [ -f "$file" ]
then
echo find media package [ $1 ].
else
echo media package [ $1 ] not found, downloading...
curl -o $GKENGINE_HOME/media/$1.7z https://raw.githubusercontent.com/gameknife/gkengine-resource/master/media/$1.7z
fi
}

unzipmedia(){
file=$GKENGINE_HOME/media/$1.7z
if [ -f "$file" ]
then
echo find media package [ $1 ]. extracting...
$GKENGINE_HOME/tools/7zr x -y $GKENGINE_HOME/media/$1.7z
else
echo not found media package [ $1 ]. go next
fi
}

downloadmedia terrian
downloadmedia textures
downloadmedia basic

unzipmedia terrian
unzipmedia textures
unzipmedia basic

echo .
echo .
echo -------------------------------------------------------------------------
echo Default Media Pack Download and Extracted.
echo Now choose optional media pack.
echo -------------------------------------------------------------------------
echo .

if [ "x$autoinstall" = "x" ]
then

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

fi

if [ $install_conf_room -eq 1 ]
then
downloadmedia conf_room
unzipmedia conf_room
fi

if [ $install_outdoor -eq 1 ]
then
downloadmedia outdoor
unzipmedia outdoor
fi

if [ $install_character -eq 1 ]
then
downloadmedia character
unzipmedia character
fi

echo .
echo .
echo -------------------------------------------------------------------------
echo Optinal media pack download and extracted.
echo Now transform to windows dds format....
echo -------------------------------------------------------------------------
echo .

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