source set_global_env.sh

listsdkmd5(){
file=$GKENGINE_HOME/../code/thirdparty/$1.7z

hash="$(echo -n $file | md5)"

echo $hash
}

listsdkmd5 freetype
listsdkmd5 havoksdk
listsdkmd5 misc
listsdkmd5 oglsdk
listsdkmd5 havoksdk_ios
