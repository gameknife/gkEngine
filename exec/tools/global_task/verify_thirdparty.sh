source set_global_env.sh

verifyresult=1

verifysdk(){
file=$GKENGINE_HOME/../code/thirdparty/$1.7z
hash=`md5 -q $file`
if [ ! $hash = $2 ]
then
verifyresult=0
#delete broken file
rm -rf $file
fi
}

verifysdk freetype      8d04c711e4ba7e551585b941a3e87fc1
verifysdk havoksdk      2027cb6828420778520c13024d8a0552
verifysdk misc          3cbdbf797ac871c9aa77a9f848e1e748
verifysdk oglsdk        e9a9ed1044204a7813ecd1f9bf78d8d5
verifysdk havoksdk_ios  85adf6e1a372b6d9f2fb2c0b89620ced

echo $verifyresult
