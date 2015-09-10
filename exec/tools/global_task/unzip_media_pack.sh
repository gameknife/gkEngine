cd ..\global_task\
source set_global_env.sh

restore=${PWD}
mkdir $GKENGINE_HOME/media
cd $GKENGINE_HOME/media

$GKENGINE_HOME/tools/7zr x $GKENGINE_HOME/media/media.7z
cd $restore
del $GKENGINE_HOME/media/media.7z
