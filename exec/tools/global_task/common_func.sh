downloadsdk(){
file=$GKENGINE_HOME/../code/thirdparty/$1.7z
if [ -f "$file" ]
then
echo find sdk package [ $1 ].
else
echo sdk package [ $1 ] not found, downloading...
curl -o $GKENGINE_HOME/../code/thirdparty/$1.7z https://raw.githubusercontent.com/gameknife/gkEngine-Resource/master/dependency/$1.7z
fi
}

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

verifymd5(){
file=$1
hash=`md5 -q $file`
if [ ! $hash = $2 ]
then
verifyresult=0
#delete broken file
rm -rf $file
fi
}

verifysdk(){
verifymd5 $GKENGINE_HOME/../code/thirdparty/$1.7z
}
