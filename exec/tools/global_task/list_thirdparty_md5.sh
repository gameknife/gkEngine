source set_global_env.sh

listsdkmd5(){
file=$GKENGINE_HOME/../code/thirdparty/$1.7z

hash=`md5 -q $file`

echo $hash
}

listsdkmd5 freetype
listsdkmd5 misc
listsdkmd5 oglsdk
