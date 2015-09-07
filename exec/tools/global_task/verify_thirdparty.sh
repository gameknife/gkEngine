source set_global_env.sh

verifyresult=1

verifysdk(){
file=$GKENGINE_HOME/../code/thirdparty/$1.7z
hash="$(echo -n $file | md5)"
if [ ! $hash = $2 ]
then
verifyresult=0
#delete broken file
rm -rf $file
fi
}

verifysdk freetype      8b4f4f614000aa2046e277d36466c44f
verifysdk havoksdk      31bc7fb22bdf8e56b492068a1d3c7437
verifysdk misc          e84a74080aa51e445486ed0e337a4996
verifysdk oglsdk        34a8ab87d4473b74d43cca391ab67b92
verifysdk havoksdk_ios  4c8887685b17b922509f77fe872f4e85

echo $verifyresult
