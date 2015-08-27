source set_global_env.sh

dir=$GKENGINE_HOME/../code/thirdparty
if [ ! -d "$dir" ]
then
mkdir $GKENGINE_HOME/../code/thirdparty
else
echo
fi
cd $GKENGINE_HOME/../code/thirdparty

downloadsdk(){
file=$GKENGINE_HOME/../code/thirdparty/$1.7z
if [ -f "$file" ]
then
echo find sdk package [ $1 ].
else
echo sdk package [ $1 ] not found, downloading...
curl -o $GKENGINE_HOME/../code/thirdparty/$1.7z https://raw.githubusercontent.com/gameknife/gkengine-resource/master/dependency/$1.7z
fi
}

downloadsdk freetype
downloadsdk havoksdk
downloadsdk misc
downloadsdk oglsdk
downloadsdk havoksdk_ios

#pause