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
if [ "${file##*.}" = "obj" ];
then

if [ -f "${file%.*}.gmf"  ]
then
echo "${file%.*}.gmf exist.";
else
$GKENGINE_HOME/tools/gkResourceCompiler -i $file -o ${file%.*}.gmf -l 5 -p 0-i
fi

fi


fi
done
}

foreachd "$GKENGINE_HOME"

#for /R %GKENGINE_HOME% %%A in (*.obj) do (
#    
#    IF NOT EXIST %%~dpnA.gmf (
#    	echo [convert file] %%~nxA to %%~nA.gmf
#    	%GKENGINE_HOME%\tools\gkResourceCompiler -i %%~fA -o %%~dpnA.gmf -l 5 -p 0-i
#    ) ELSE (
#    	echo [file exist] %%~nA.gmf
#    )
#
#)

# pause