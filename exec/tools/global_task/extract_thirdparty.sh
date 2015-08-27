source set_global_env.sh

if []
mkdir $GKENGINE_HOME/../code/thirdparty
cd $GKENGINE_HOME/../code/thirdparty

downloadsdk(){
file=$GKENGINE_HOME/../code/thirdparty/$0.7z
if [ -f "$file" ]
then
echo find sdk.
else
curl -o $GKENGINE_HOME/../code/thirdparty/$0.7z https://raw.githubusercontent.com/gameknife/gkengine-resource/master/dependency/$0.7z
fi
}

downloadsdk dxsdk

#pause