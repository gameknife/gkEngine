cd ../global_task/
source set_global_env.sh

foreachd(){
# 遍历参数1
for file in $1/*
do
# 如果是目录就打印处理，然后继续遍历，递归调用
if [ -d $file ]
then
#echo $file"是目录"
foreachd $file
elif [ -f $file ]
then
#echo $file pvr process
if [ "${file##*.}" = "tga" ];
then
echo $file
#$GKENGINE_HOME/tools/pvrtextool -m -f a8r8g8b8 -shh -i $file -o ${file%.*}.dds
#$GKENGINE_HOME/tools/convert $file ${file%.*}.dds
$GKENGINE_HOME/tools/nvcompress -bc3 $file ${file%.*}.dds
fi


fi
done
}

foreachd "$GKENGINE_HOME/media"