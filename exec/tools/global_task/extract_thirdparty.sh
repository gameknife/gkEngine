source set_global_env.sh
source common_func.sh

dir=$GKENGINE_HOME/../code/thirdparty
if [ ! -d "$dir" ]
then
mkdir $GKENGINE_HOME/../code/thirdparty
else
echo
fi
cd $GKENGINE_HOME/../code/thirdparty



downloadsdk freetype
downloadsdk havoksdk
downloadsdk misc
downloadsdk oglsdk
downloadsdk havoksdk_ios

#pause