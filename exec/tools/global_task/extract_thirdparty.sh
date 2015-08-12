sh ../global_task/set_global_env.sh

mkdir $GKENGINE_HOME/../code/thirdparty
cd $GKENGINE_HOME/../code/thirdparty

#%GKENGINE_HOME%\tools\wget http://www.gameknife.cc/gkENGINE_RES/depends.7z
$GKENGINE_HOME/tools/7zr x $GKENGINE_HOME/../code/thirdparty/depends.7z

#del %GKENGINE_HOME%\..\code\thirdparty\depends.7z

#pause