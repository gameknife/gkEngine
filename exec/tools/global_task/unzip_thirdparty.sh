source set_global_env.sh

cd $GKENGINE_HOME/../code/thirdparty

unzipsdk(){
file=$GKENGINE_HOME/../code/thirdparty/$1.7z
if [ -f "$file" ]
then
echo find sdk package [ $1 ]. extracting...
$GKENGINE_HOME/tools/7zr x -y $GKENGINE_HOME/../code/thirdparty/$1.7z
else
echo not found sdk package [ $1 ]. go next
fi
}

unzipsdk freetype
unzipsdk misc
unzipsdk oglsdk

#cd %restore%
#del %GKENGINE_HOME%\..\code\thirdparty\depends.7z
