source set_global_env.sh

dir=$GKENGINE_HOME/../code/thirdparty
if [ ! -d "$dir" ]
then
mkdir $GKENGINE_HOME/../code/thirdparty
else
echo
fi
cd $GKENGINE_HOME/../code/thirdparty

source common_func.sh

downloadsdk freetype
downloadsdk havoksdk
downloadsdk misc
downloadsdk oglsdk
downloadsdk havoksdk_ios

#pause