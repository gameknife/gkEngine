sh ../global_task/set_global_env.sh

#set restore=%cd%
echo $GKENGINE_HOME

mkdir $GKENGINE_HOME/../code/thirdparty
cd $GKENGINE_HOME/../code/thirdparty
$GKENGINE_HOME/tools/7zr x $GKENGINE_HOME/../code/thirdparty/depends.7z
#cd %restore%
#del %GKENGINE_HOME%\..\code\thirdparty\depends.7z
